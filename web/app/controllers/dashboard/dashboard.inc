<?php

function dashboard_access($user) {
  return TRUE;
}

function dashboard_show_execute() {
  $data = json_decode(file_get_contents('dashboard/data.json'));
  return array('content' => $data);
}
