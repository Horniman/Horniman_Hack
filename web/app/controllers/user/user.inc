<?php

function user_access($user) {
  global $request;
  // Always allow access to logout
  if ($request['op'] == 'logout') {
    return TRUE;
  }
  // Only allow POST requests to the login operation
  if ($request['op'] == 'login') {
    return $_SERVER['REQUEST_METHOD'] == 'POST';
  }
  return FALSE;
}

function user_logout_execute() {
  session_destroy();
  leak_goto();
}

function user_login_execute() {
  global $user, $db;

  $utest = $db->prepare("SELECT * FROM auth_user WHERE password = ? AND username = ?");
  $utest->execute(array(sha1($_POST['pass']), $_POST['name']));
  $users = $utest->fetchAll();

  if (!empty($users)) {
    $user = $users[0];
    $_SESSION['user'] = $user;
    leak_goto();
  }
  else {
    return leak_access_denied();
  }
}


