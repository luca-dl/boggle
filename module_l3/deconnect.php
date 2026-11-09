<?php
require 'config.php';

// Déconnexion en POST + CSRF : un lien piégé ne peut pas déconnecter l'utilisateur
if ($_SERVER['REQUEST_METHOD'] === 'POST' && csrf_ok()) {
    $_SESSION = [];
    session_destroy();
}
header('Location: index.php');
exit;
