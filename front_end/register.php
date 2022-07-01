<?php
  header("content-type:text/html;charset=utf-8");
  error_reporting(E_ALL &~ E_NOTICE);
  $username = $_POST['username'];
  $password = $_POST['password'];
  $name = $_POST['name'];
  $mailAddr = $_POST['mailAddr'];
  $command = "register -u ".$username." -p ".$password." -n ".$name." -m ".$mailAddr;
  $socket = socket_create(AF_INET, SOCK_STREAM, SOL_TCP);
  socket_connect($socket, '127.0.0.1', 7777);
  socket_write($socket, $command);
  $res = socket_read($socket, 1048576);
  echo $res;
?>