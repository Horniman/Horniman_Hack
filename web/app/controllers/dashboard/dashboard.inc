<?php

function dashboard_access($user) {
  return TRUE;
}

function dashboard_show_execute() {
  $data = json_decode(file_get_contents('controllers/dashboard/data.json'));
  return array('content' => $data);
}
