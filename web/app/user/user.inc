<?php

function user_access($user) {
  return TRUE;
}

function user_login_execute() {
  return array('content' => 'Logged in');
}
