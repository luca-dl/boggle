<?php
require 'config.php';

if (!empty($_SESSION['user_id'])) { header('Location: parties.php'); exit; }

$vues = ['connexion', 'inscription', 'recuperation'];
$vue = $_SERVER['REQUEST_METHOD'] === 'POST' ? post('action') : ($_GET['vue'] ?? 'connexion');
if (!in_array($vue, $vues, true)) $vue = 'connexion';

$erreur = '';
$succes = flash();
$lienDev = '';

if ($_SERVER['REQUEST_METHOD'] === 'POST') {

    if (!csrf_ok()) {
        $erreur = 'Formulaire expiré, merci de réessayer.';

    // ---------- CONNEXION ----------
    } elseif ($vue === 'connexion') {
        $ip = ip_client();
        $stmt = $pdo->prepare('SELECT COUNT(*) FROM tentatives WHERE ip = ? AND quand > ?');
        $stmt->execute([$ip, maintenant(-FENETRE_TENTATIVES)]);

        if ($stmt->fetchColumn() >= MAX_TENTATIVES) {
            $erreur = 'Trop de tentatives échouées. Réessayez dans 15 minutes.';
        } else {
            $identifiant = trim(post('identifiant'));
            $stmt = $pdo->prepare('SELECT * FROM utilisateurs WHERE LOWER(pseudo) = LOWER(?) OR email = ?');
            $stmt->execute([$identifiant, strtolower($identifiant)]);
            $user = $stmt->fetch();

            if ($user && password_verify(post('mdp'), $user['mot_de_passe'])) {
                $pdo->prepare('DELETE FROM tentatives WHERE ip = ?')->execute([$ip]);
                session_regenerate_id(true);   // anti fixation de session
                $_SESSION['user_id'] = $user['id'];
                $_SESSION['pseudo'] = $user['pseudo'];
                $_SESSION['derniere_activite'] = time();
                header('Location: parties.php');
                exit;
            }
            $pdo->prepare('INSERT INTO tentatives (ip, quand) VALUES (?, ?)')->execute([$ip, maintenant()]);
            $erreur = 'Identifiant ou mot de passe incorrect.';
        }

    // ---------- INSCRIPTION ----------
    } elseif ($vue === 'inscription') {
        $pseudo = trim(post('pseudo'));
        $email = strtolower(trim(post('email')));
        $mdp = post('mdp');

        if (!preg_match('/^[a-zA-Z0-9_-]{3,30}$/', $pseudo)) {
            $erreur = 'Pseudo : 3 à 30 caractères (lettres, chiffres, - ou _).';
        } elseif (!filter_var($email, FILTER_VALIDATE_EMAIL)) {
            $erreur = 'Adresse e-mail invalide.';
        } elseif (!mdp_valide($mdp)) {
            $erreur = 'Mot de passe : 8 caractères minimum, avec au moins une lettre et un chiffre.';
        } elseif ($mdp !== post('mdp2')) {
            $erreur = 'Les mots de passe ne correspondent pas.';
        } else {
            $stmt = $pdo->prepare('SELECT 1 FROM utilisateurs WHERE LOWER(pseudo) = LOWER(?) OR email = ?');
            $stmt->execute([$pseudo, $email]);
            if ($stmt->fetch()) {
                $erreur = 'Ce pseudo ou cet e-mail est déjà utilisé.';
            } else {
                $pdo->prepare('INSERT INTO utilisateurs (pseudo, email, mot_de_passe, cree_le) VALUES (?, ?, ?, ?)')
                    ->execute([$pseudo, $email, password_hash($mdp, PASSWORD_DEFAULT), maintenant()]);
                flash('✅ Compte créé ! Vous pouvez vous connecter.');
                header('Location: index.php');
                exit;
            }
        }

    // ---------- MOT DE PASSE OUBLIÉ ----------
    } elseif ($vue === 'recuperation') {
        $email = strtolower(trim(post('email')));
        $stmt = $pdo->prepare('SELECT id FROM utilisateurs WHERE email = ?');
        $stmt->execute([$email]);
        $user = $stmt->fetch();

        if ($user) {
            $token = bin2hex(random_bytes(32));   // 256 bits aléatoires
            $pdo->prepare('UPDATE utilisateurs SET token_reset = ?, token_expire = ? WHERE id = ?')
                ->execute([hash('sha256', $token), maintenant(3600), $user['id']]);
            $lien = BASE_URL . '/reinitialiser.php?token=' . $token;
            @mail($email, 'Boggle - Réinitialisation du mot de passe', "Lien valable 1 heure :\n$lien");
            if (MODE_DEV) $lienDev = $lien;
        }
        // Même message dans tous les cas → on ne révèle pas quels e-mails existent
        $succes = 'Si un compte existe avec cet e-mail, un lien de réinitialisation a été envoyé.';
    }
}

// Grille de démonstration : le mot « JOUER » est surligné
$lettres = str_split('JOUESATRMINEBLOC');
$chemin = [0, 1, 2, 3, 7];
?>
<!DOCTYPE html>
<html lang="fr">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>Boggle – Le jeu de lettres en ligne</title>
  <link rel="stylesheet" href="style.css">
</head>
<body>
<div class="accueil">

  <section class="presentation">
    <h1 class="logo"><span>B</span><span>O</span><span>G</span><span>G</span><span>L</span><span>E</span></h1>
    <p class="slogan">Trouvez un maximum de mots en 3 minutes !</p>

    <div class="grille-demo" aria-hidden="true">
      <?php foreach ($lettres as $i => $l): ?>
        <div class="de<?= in_array($i, $chemin, true) ? ' surligne' : '' ?>"><?= $l ?></div>
      <?php endforeach; ?>
    </div>

    <ul class="regles">
      <li>🎲 16 dés mélangés dans une grille 4×4</li>
      <li>🔗 Reliez des lettres voisines pour former des mots</li>
      <li>⏱️ 3 minutes pour marquer un maximum de points</li>
      <li>🏆 Affrontez vos amis et grimpez au classement</li>
    </ul>
  </section>

  <main class="carte">
    <div class="onglets">
      <a class="onglet<?= $vue === 'connexion' ? ' actif' : '' ?>" href="?vue=connexion">Connexion</a>
      <a class="onglet<?= $vue === 'inscription' ? ' actif' : '' ?>" href="?vue=inscription">Créer un compte</a>
    </div>

    <?php if ($erreur): ?><p class="message erreur"><?= e($erreur) ?></p><?php endif; ?>
    <?php if ($succes): ?><p class="message succes"><?= e($succes) ?></p><?php endif; ?>
    <?php if ($lienDev): ?>
      <p class="message dev">🛠️ Mode démo : <a href="<?= e($lienDev) ?>">réinitialiser le mot de passe</a></p>
    <?php endif; ?>

    <?php if ($vue === 'connexion'): ?>
      <form method="post" class="formulaire">
        <?= csrf_champ() ?>
        <input type="hidden" name="action" value="connexion">
        <label>Pseudo ou e-mail
          <input type="text" name="identifiant" required autocomplete="username" value="<?= e(post('identifiant')) ?>">
        </label>
        <label>Mot de passe
          <input type="password" name="mdp" required autocomplete="current-password">
        </label>
        <button class="btn">Se connecter</button>
        <a href="?vue=recuperation" class="lien">Mot de passe oublié ?</a>
      </form>

    <?php elseif ($vue === 'inscription'): ?>
      <form method="post" class="formulaire">
        <?= csrf_champ() ?>
        <input type="hidden" name="action" value="inscription">
        <label>Pseudo
          <input type="text" name="pseudo" required minlength="3" maxlength="30" pattern="[a-zA-Z0-9_\-]{3,30}"
                 autocomplete="username" value="<?= e(post('pseudo')) ?>">
        </label>
        <label>E-mail
          <input type="email" name="email" required autocomplete="email" value="<?= e(post('email')) ?>">
        </label>
        <label>Mot de passe <small>(8 caractères min., lettre + chiffre)</small>
          <input type="password" name="mdp" required minlength="8" maxlength="72" autocomplete="new-password">
        </label>
        <label>Confirmer le mot de passe
          <input type="password" name="mdp2" required autocomplete="new-password">
        </label>
        <button class="btn">Créer mon compte</button>
      </form>

    <?php else: ?>
      <form method="post" class="formulaire">
        <?= csrf_champ() ?>
        <input type="hidden" name="action" value="recuperation">
        <h2 class="titre">Mot de passe oublié</h2>
        <p class="info">Entrez l'e-mail de votre compte, nous vous enverrons un lien valable 1 heure.</p>
        <label>E-mail
          <input type="email" name="email" required autocomplete="email">
        </label>
        <button class="btn">Envoyer le lien</button>
        <a href="?vue=connexion" class="lien">← Retour à la connexion</a>
      </form>
    <?php endif; ?>
  </main>

</div>
</body>
</html>
