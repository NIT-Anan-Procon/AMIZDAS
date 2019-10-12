<?php
require_once('../Config/SQLServer.php');
$mysqli =  MySQLi();
if($mysqli->connect_errno){
    echo $mysqli->connect_errno.";".$mysqli->connect_errnor;
} 
mysqli_set_charset($mysqli,'utf8');
$id = $_POST['address'];
if(isset($_GET['name'])){
    $name =$_GET['name'];
    $select_module ="select module_ID as module_ID from water_level where water_level_name = '$name'";
    foreach($mysqli->query($select_module) as $module){
        $module_ID= $module['module_ID'];
    }
}
function Get_flag($flag){
    if(!($flag==NULL)){
        foreach($flag as $row){
            if($row==0){
                return 0;
            }
        }
    }
    return 1;
}
if($_POST["update"]){
    // var_dump($id);
    foreach($id as $row){
        $selectAD ="select mailaddress from address where mailaddress ='$row'";
        $CountAD = $mysqli->query($selectAD);
        if($CountAD->fetch_assoc()){        
            // var_dump($CountAD->fetch_assoc());    
        }else{
            $insertAD ="insert into address(mailaddress) values('$row')";
            $stmt = $mysqli->query($insertAD);
            
            echo("メールアドレスを登録しました");
        }
        $select ="select address_ID as AD_ID from address where mailaddress='$row'";
        foreach($mysqli->query($select) as $addressID){ 
            $AD_ID=$addressID['AD_ID'];
            //$selectAD ="select mailaddress from address where mailaddress ='116720@st.anan-nct.ac.jp'";
            
            $selectCa ="select address_ID from alert where address_ID ='$AD_ID'";
            $selectCm ="select module_ID from alert where module_ID ='$module_ID'";
            $selectFl ="select flag from alert where address_ID ='$AD_ID' and module_ID ='$module_ID'";
            $selectAl ="select count(*) as num from alert";
            $CountCa = $mysqli->query($selectCa);
            $CountCm = $mysqli->query($selectCm);
            $CountFl = $mysqli->query($selectFl);
            $CountAl = $mysqli->query($selectAl);
            $flag=array();
            foreach($CountFl as $row){
                array_push($flag,$row['flag']);
            }
            $AlertNum=$CountAl->fetch_assoc();
                // if($test == 0){

                //     echo "正解や";
                // }else{
                //     echo "ダメです";
                // }
            //var_dump($CountCa);
            if(!($AlertNum==0)){
                if(Get_flag($flag)==0){
                    if($CountCa->fetch_assoc()){
                        if($CountCm->fetch_assoc()){
                            $judge=1;
                            
                        }else{
                            $judge=0;
                        
                        }
                    }else{
                        $judge=0;
                        
                    }
                }else{
                    $judge=0;
                    
                }
            }else{
                $judge==0;
            }
            $attention = $_POST['attention'];
            $danger = $_POST['danger'];
            $insert = "insert into alert(module_ID,address_ID,attention,danger,flag) values('$module_ID','$AD_ID','$attention','$danger',0)";
            $update = "update alert set attention = '$attention',danger ='$danger' where module_ID = '$module_ID' and address_ID ='$AD_ID' and flag=0";
            if($judge==1){
            $stmt2 =$mysqli->query($update);
            }else{
                $stmt2 =$mysqli->query($insert);
            }
        }
    }
    $url = "../complete_con.html?name=".$name;
             header("Location:" .$url);
}


?>