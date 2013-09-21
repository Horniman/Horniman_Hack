<?php

function test_access($user) {
  return !empty($user['uid']);
}

function test_show_execute() {
  return array('content' => 'This is just a test callback.');
}
