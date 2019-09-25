<?php
session_start();
  require_once('Config/SQLServer.php');
  $mysqli =  MySQLi();
  if($mysqli->connect_errno){
  echo $mysqli->connect_errno.";".$mysqli->connect_errnor;
  } 
 mysqli_set_charset($mysqli,'utf8');
$error_message = "";
if(isset($_POST["login"]) && $_POST["login"]){
	$address= $_REQUEST['mailaddress'];
    $select ="select count(mailaddress) as num from address where mailaddress ='$address'";
    $select2 ="select address_ID from address where mailaddress='$address'";
    $stmt =$mysqli->query($select);
    $stmt2 =$mysqli->query($select2);
   foreach($stmt as $row){
       foreach($stmt2 as $row2){
           if($row['num']==1){
                $_SESSION["mailaddress"] = $_REQUEST["mailaddress"];
                $addressID=$row2['address_ID'];
                $url = "mypage.html?ID=".$addressID;
                header("Location:" .$url);
                exit;
            }
        }
   }
   $error_message ="ID,パスワードが間違っています。";
	
		
}
?>
<!DOCTYPE html>
<html>
	<head>
    <meta charset="utf-8">
    <link rel="stylesheet" href="css/login.css">
	<link rel="stylesheet" href="css/config.css">
    <link rel="stylesheet" href="css/menu.css">
    <link rel="stylesheet" href="css/drawer.min.css">
    <script src="js/jquery.min.js"></script>
    <script src="js/iscroll.js"></script>
    <script src="js/drawer.min.js"></script>
    <script type="text/javascript" src="js/menu.js"></script>

    <title>AMIZDAS Login</title>
  </head>

  <body class="drawer drawer--right">
    <button type="button" class="drawer-toggle drawer-hamburger">
      <span class="sr-only">toggle navigation</span>
      <span class="drawer-hamburger-icon"></span>
    </button>
    <nav class="drawer-nav">
      <ul class="drawer-menu">
        <p>メニュー</p>
        <li><a href="index.html">topページへ</a></li>
        <li><a href="login_mypage.php">マイページへ</a></li>
        <li><a href="login_admin.php">管理者ページへ</a></li>
      </ul>
    </nav>

    <div class="minititle">
	     マイページログイン
    </div>
		<div class="conf">
        <p>水位計のアラートを設定しているメールアドレスを入力してください。</p>
        <form action="login_mypage.php" method="POST">
			<tr>
   				<td><b> ユーザID(メールアドレス)：</b></td>
    				<td><input type="text" name="mailaddress"></td>
  			</tr>
      <br>
			<div class="button">
        			<input type="submit" name="login" value="ログイン">
              <a href="javascript:history.back()"><input type="button"  value="キャンセル"></a>
        </form>
	  		</div>
		</div>
	<body>
</html>

<?php
if($error_message){
echo $error_message;
}
?>