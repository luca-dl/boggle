<?php
date_default_timezone_set('Europe/Paris');

// Render est derrière un proxy : HTTPS détecté via cet en-tête
$https = ($_SERVER['HTTP_X_FORWARDED_PROTO'] ?? '') === 'https' || !empty($_SERVER['HTTPS']);

// ===== En-têtes de sécurité HTTP =====
header('X-Frame-Options: DENY');              // anti-clickjacking
header('X-Content-Type-Options: nosniff');
header('Referrer-Policy: no-referrer');       // le token de reset ne fuit pas via le Referer
header("Content-Security-Policy: default-src 'self'; frame-ancestors 'none'; form-action 'self'");
if ($https) header('Strict-Transport-Security: max-age=31536000');

// ===== Session sécurisée =====
session_set_cookie_params([
    'path' => '/', 'httponly' => true, 'secure' => $https, 'samesite' => 'Lax',
]);
session_start();

define('MODE_DEV', getenv('MODE_DEV') === '1');   // affiche le lien de reset (démo)
// APP_URL fixée sur Render → évite l'injection d'en-tête Host dans le lien de reset
define('BASE_URL', rtrim(getenv('APP_URL') ?: (($https ? 'https' : 'http') . '://' . ($_SERVER['HTTP_HOST'] ?? 'localhost')), '/'));
define('DUREE_SESSION', 30 * 60);       // déconnexion après 30 min d'inactivité
define('MAX_TENTATIVES', 5);            // 5 échecs de connexion max…
define('FENETRE_TENTATIVES', 15 * 60);  // …par tranche de 15 min

function erreur_fatale($msg) {
    http_response_code(503);
    exit('<!DOCTYPE html><html lang="fr"><head><meta charset="UTF-8"><title>Boggle</title>'
       . '<link rel="stylesheet" href="style.css"></head><body><main class="carte">'
       . '<p class="message erreur">' . $msg . '</p></main></body></html>');
}

// ===== Base de données : Neon si DATABASE_URL existe, sinon SQLite =====
try {
    $url = getenv('DATABASE_URL');
    if ($url) {
        $p = parse_url($url);
        parse_str($p['query'] ?? '', $q);
        $dsn = sprintf('pgsql:host=%s;port=%d;dbname=%s;sslmode=%s',
            $p['host'], $p['port'] ?? 5432, ltrim($p['path'], '/'), $q['sslmode'] ?? 'require');
        $pdo = new PDO($dsn, urldecode($p['user']), urldecode($p['pass'] ?? ''));
        $cle = 'SERIAL PRIMARY KEY';
    } else {
        $fichier = getenv('SQLITE_PATH') ?: dirname(__DIR__) . '/data/boggle.sqlite';
        if (!is_dir(dirname($fichier))) mkdir(dirname($fichier), 0700, true);
        $pdo = new PDO('sqlite:' . $fichier);
        $cle = 'INTEGER PRIMARY KEY AUTOINCREMENT';
    }
    $pdo->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);
    $pdo->setAttribute(PDO::ATTR_DEFAULT_FETCH_MODE, PDO::FETCH_ASSOC);

    // Création automatique des tables
    $pdo->exec("CREATE TABLE IF NOT EXISTS utilisateurs (
        id $cle,
        pseudo VARCHAR(30) NOT NULL,
        email VARCHAR(255) NOT NULL UNIQUE,
        mot_de_passe VARCHAR(255) NOT NULL,
        token_reset VARCHAR(64),
        token_expire VARCHAR(19),
        cree_le VARCHAR(19) NOT NULL
    )");
    $pdo->exec('CREATE UNIQUE INDEX IF NOT EXISTS pseudo_unique ON utilisateurs (LOWER(pseudo))');
    $pdo->exec("CREATE TABLE IF NOT EXISTS tentatives (
        id $cle, ip VARCHAR(45) NOT NULL, quand VARCHAR(19) NOT NULL
    )");
} catch (PDOException $ex) {
    error_log($ex->getMessage());   // visible dans les logs Render, jamais à l'écran
    erreur_fatale('❌ Base de données indisponible.');
}

// ===== Fonctions utiles =====
function e($t) { return htmlspecialchars((string) ($t ?? ''), ENT_QUOTES, 'UTF-8'); }

function post($cle) {   // récupère un champ POST en garantissant une chaîne
    $v = $_POST[$cle] ?? '';
    return is_string($v) ? $v : '';
}

function maintenant($decalage = 0) { return date('Y-m-d H:i:s', time() + $decalage); }

function flash($msg = null) {
    if ($msg !== null) { $_SESSION['flash'] = $msg; return ''; }
    $m = $_SESSION['flash'] ?? '';
    unset($_SESSION['flash']);
    return $m;
}

// Jeton CSRF : empêche un autre site de soumettre nos formulaires
function csrf_champ() {
    if (empty($_SESSION['csrf'])) $_SESSION['csrf'] = bin2hex(random_bytes(32));
    return '<input type="hidden" name="csrf" value="' . $_SESSION['csrf'] . '">';
}
function csrf_ok() {
    return !empty($_SESSION['csrf']) && hash_equals($_SESSION['csrf'], post('csrf'));
}

// 8 à 72 caractères (limite de bcrypt), au moins une lettre et un chiffre
function mdp_valide($m) {
    return strlen($m) >= 8 && strlen($m) <= 72
        && preg_match('/[a-zA-Z]/', $m) && preg_match('/[0-9]/', $m);
}

function ip_client() {
    $xff = $_SERVER['HTTP_X_FORWARDED_FOR'] ?? '';
    $ip = trim(explode(',', $xff)[0]) ?: ($_SERVER['REMOTE_ADDR'] ?? '');
    return substr($ip, 0, 45);
}

function exiger_connexion() {
    if (empty($_SESSION['user_id'])) { header('Location: index.php'); exit; }
    if (time() - ($_SESSION['derniere_activite'] ?? 0) > DUREE_SESSION) {
        $_SESSION = [];
        session_regenerate_id(true);
        flash('⏱️ Session expirée, reconnectez-vous.');
        header('Location: index.php');
        exit;
    }
    $_SESSION['derniere_activite'] = time();
}
