<?php
require 'config.php';

$token = $_GET['token'] ?? post('token');
$token = is_string($token) ? $token : '';
$erreur = '';
$user = false;

if ($token !== '') {
    $stmt = $pdo->prepare('SELECT id FROM utilisateurs WHERE token_reset = ? AND token_expire > ?');
    $stmt->execute([hash('sha256', $token), maintenant()]);
    $user = $stmt->fetch();
}

if ($user && $_SERVER['REQUEST_METHOD'] === 'POST') {
    $mdp = post('mdp');
    if (!csrf_ok()) {
        $erreur = 'Formulaire expiré, merci de réessayer.';
    } elseif (!mdp_valide($mdp)) {
        $erreur = 'Mot de passe : 8 caractères minimum, avec au moins une lettre et un chiffre.';
    } elseif ($mdp !== post('mdp2')) {
        $erreur = 'Les mots de passe ne correspondent pas.';
    } else {
        // Token supprimé → lien utilisable une seule fois
        $pdo->prepare('UPDATE utilisateurs SET mot_de_passe = ?, token_reset = NULL, token_expire = NULL WHERE id = ?')
            ->execute([password_hash($mdp, PASSWORD_DEFAULT), $user['id']]);
        flash('✅ Mot de passe modifié ! Connectez-vous.');
        header('Location: index.php');
        exit;
    }
}
?>
<!DOCTYPE html>
<html lang="fr">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>Boggle – Nouveau mot de passe</title>
  <link rel="stylesheet" href="style.css">
</head>
<body>
<main class="carte">
  <h1 class="logo"><span>B</span><span>O</span><span>G</span><span>G</span><span>L</span><span>E</span></h1>
  <h2 class="titre">Nouveau mot de passe</h2>

  <?php if (!$user): ?>
    <p class="message erreur">Lien invalide ou expiré.</p>
    <a href="index.php?vue=recuperation" class="lien">Refaire une demande</a>
  <?php else: ?>
    <?php if ($erreur): ?><p class="message erreur"><?= e($erreur) ?></p><?php endif; ?>
    <form method="post" class="formulaire">
      <?= csrf_champ() ?>
      <input type="hidden" name="token" value="<?= e($token) ?>">
      <label>Nouveau mot de passe
        <input type="password" name="mdp" required minlength="8" maxlength="72" autocomplete="new-password">
      </label>
      <label>Confirmer
        <input type="password" name="mdp2" required autocomplete="new-password">
      </label>
      <button class="btn">Réinitialiser</button>
    </form>
  <?php endif; ?>
</main>
</body>
</html>
