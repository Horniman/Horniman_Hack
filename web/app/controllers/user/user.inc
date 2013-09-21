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
  coral_goto();
}

function user_login_execute() {
  global $user, $db, $config;

  $utest = $db->prepare("SELECT * FROM acl_user
    WHERE password = ? AND name = ?");
  $utest->execute(array(
    sha1($_POST['pass'] . $config['salt']),
    $_POST['name']
  ));
  $users = $utest->fetchAll();

  if (!empty($users)) {
    $user = $users[0];
    $_SESSION['user'] = $user;
    coral_goto();
  }
  else {
    coral_set_message('Username or password unknown');
    return coral_access_denied();
  }
}


