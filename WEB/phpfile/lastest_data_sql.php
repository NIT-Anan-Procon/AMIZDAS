<?PHP
    //sql接続
    require_once('Config/SQLServer.php');
    $mysqli =MySQLi();
    if($mysqli->connect_errno){
    echo $mysqli->connect_errno.';'.$mysqli->connect_errnor;
    }
    mysqli_set_charset( $mysqli, 'utf8');

    $file = ["phpfile/csvfile.csv","phpfile/csvfile_sakura.csv"];
    $module = ["739D7D","uGqZClHAVHB2"];
    $module_name = ['水位一号機','水位二号機'];

    for($i=0;$i<2;$i++){
        $lines = file($file[$i], FILE_IGNORE_NEW_LINES | FILE_SKIP_EMPTY_LINES);
        $water = preg_split("/[、,]/",$lines[count($lines)-1]);
        
        $sql = "UPDATE water_level SET 
                time = '$water[0]',
                water_gage = $water[1],
                radiation = $water[2],
                vattery_voltage = $water[3],
                temperature = $water[4],
                water_gage2 = $water[5],
                lat = $water[6],
                lng = $water[7]
                where module_ID = '$module[$i]'";
    
    $stmt = $mysqli->query($sql);
    }
    $mysqli->close();

?>
