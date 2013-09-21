<?php

function dashboard_access($user) {
  return !empty($user['id']);
}

function dashboard_show_execute() {
  $data = json_decode(file_get_contents('controllers/dashboard/data.json'));

  // If json just return the data
  global $request;
  if ($request['format'] == 'json') {
    return array('content' => $data);
  }

  $vars = array('content' => $data);
  $content = coral_template('dashboard', $vars, 'controllers/dashboard');
  return array(
    'page_title' => 'Dashboard',
    'content' => $content,
  );
}
