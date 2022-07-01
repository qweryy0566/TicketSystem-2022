<?php
  header("content-type:text/html;charset=utf-8");
  error_reporting(E_ALL &~ E_NOTICE);
  $command = "is_login -u ".$_GET['username'];
  $socket = socket_create(AF_INET, SOCK_STREAM, SOL_TCP);
  socket_connect($socket, '127.0.0.1', 7777);
  socket_write($socket, $command);
  $res = socket_read($socket, 1048576);
  echo $res;
?>