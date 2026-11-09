<?php
require 'config.php';
exiger_connexion();

// Données fictives (attendu de la séance)
$parties = [
    ['date' => '08/11/2026 18:42', 'joueurs' => 3, 'mots' => 27,  'score' => 41,  'statut' => 'Terminée'],
    ['date' => '07/11/2026 21:15', 'joueurs' => 2, 'mots' => 19,  'score' => 28,  'statut' => 'Terminée'],
    ['date' => '06/11/2026 12:03', 'joueurs' => 4, 'mots' => 33,  'score' => 52,  'statut' => 'Terminée'],
    ['date' => '—',                'joueurs' => 2, 'mots' => '—', 'score' => '—', 'statut' => 'En attente'],
];
$info = isset($_GET['nouvelle']) ? '🚧 La création de partie arrive bientôt !' : '';
?>
<!DOCTYPE html>
<html lang="fr">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>Boggle – Mes parties</title>
  <link rel="stylesheet" href="style.css">
</head>
<body class="page-parties">

<header class="barre">
  <div class="logo petit"><span>B</span><span>O</span><span>G</span><span>G</span><span>L</span><span>E</span></div>
  <div>
    👤 <strong><?= e($_SESSION['pseudo']) ?></strong>
    <form method="post" action="deconnexion.php" class="inline">
      <?= csrf_champ() ?>
      <button class="btn-secondaire">Déconnexion</button>
    </form>
  </div>
</header>

<main class="carte large">
  <div class="entete">
    <h1>Mes parties</h1>
    <a class="btn btn-accent" href="?nouvelle=1">+ Nouvelle partie</a>
  </div>

  <?php if ($info): ?><p class="message dev"><?= e($info) ?></p><?php endif; ?>

  <div class="tableau">
    <table>
      <thead>
        <tr><th>Date</th><th>Joueurs</th><th>Mots trouvés</th><th>Score</th><th>Statut</th></tr>
      </thead>
      <tbody>
        <?php foreach ($parties as $p): ?>
          <tr>
            <td><?= e($p['date']) ?></td>
            <td><?= e($p['joueurs']) ?></td>
            <td><?= e($p['mots']) ?></td>
            <td><strong><?= e($p['score']) ?></strong></td>
            <td><span class="badge <?= $p['statut'] === 'Terminée' ? 'ok' : 'attente' ?>"><?= e($p['statut']) ?></span></td>
          </tr>
        <?php endforeach; ?>
      </tbody>
    </table>
  </div>
</main>

</body>
</html>
