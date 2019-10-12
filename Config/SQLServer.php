<?php
function MySQLi(){
	$Mysql =new mysqli('localhost','amizdas','Amizdas.1','amizdas');//左から順に'ホスト名','ユーザー名','データーベース名'
	return $Mysql;
}
?>
