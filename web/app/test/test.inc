<?php

function test_access($user) {
  return !empty($user['id']);
}

function test_show_execute() {
  return array('content' => 'This is just a test callback.');
}
