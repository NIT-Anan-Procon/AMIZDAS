var param = location.search;
var csvfile; 
if(getParam('num')==1){
    csvfile = 'phpfile/csvfile.csv';
}else if(getParam('num')==2){
    csvfile = 'phpfile/csvfile_sakura.csv';
}else{
    csvfile = 'phpfile/csvfile.csv';
}

//クエリ文字列の特定のキーの値だけを取得
function getParam(name, url) {
    if (!url) url = window.location.href;
    name = name.replace(/[\[\]]/g, "\\$&");
    var regex = new RegExp("[?&]" + name + "(=([^&#]*)|&|#|$)"),
        results = regex.exec(url);
    if (!results) return null;
    if (!results[2]) return '';
    return decodeURIComponent(results[2].replace(/\+/g, " "));
}

function getCSV(){
    var req = new XMLHttpRequest(); // HTTPでファイルを読み込むためのXMLHttpRrequestオブジェクトを生成
    req.open("get",csvfile, true); // アクセスするファイルを指定
    req.send(null); // HTTPリクエストの発行

    // レスポンスが返ってきたらconvertCSVtoArray()を呼ぶ	
    req.onload = function(){
    convertCSVtoArray(req.responseText); // 渡されるのは読み込んだCSVデータ
    }
}
// 読み込んだCSVデータを二次元配列に変換する関数convertCSVtoArray()の定義
function convertCSVtoArray(str){ // 読み込んだCSVデータが文字列として渡される
    var result = []; // 最終的な二次元配列を入れるための配列
    var tmp = str.split("\n"); // 改行を区切り文字として行を要素とした配列を生成

    // 各行ごとにカンマで区切った文字列を要素とした二次元配列を生成
    for(var i=0;i<tmp.length;++i){
        result[i] = tmp[i].split(',');
    }
    water_level(result);
}


//二次元配列から水位と時間だけを配列を作成
function water_level(str){
    var data = str;
    var water = [];
    var time = [];
    for(var i=0;i<data.length-1;i++){
        water.push(data[i][1]);
        time.push(data[i][0]);
    }
    creatgraph(water,time);
}


//グラフの作成
function creatgraph(mizu,zikan){
    var data = mizu;
    var time = zikan;
    var ctx = document.getElementById("myLine2Chart");
    var myLine2Chart = new Chart(ctx, {
    //グラフの種類
    type: 'line',
    //データの設定
    data: {
        //データ項目のラベル
        labels: time,
        //データセット
        datasets: [
            {
                label: "水位",
                fill: false,
                lineTension: 0.1,
                backgroundColor: "rgba(75,192,192,0.4)",
                borderColor: "rgb(5,141,199)",
                borderCapStyle: 'butt',
                borderDash: [],
                borderDashOffset: 0.0,
                borderJoinStyle: 'miter',
                pointBorderColor: "rgb(5,141,199)",
                pointBackgroundColor: "#fff",
                pointBorderWidth: 1,
                pointHoverRadius: 5,
                pointHoverBackgroundColor: "rgba(75,192,192,1)",
                pointHoverBorderColor: "rgba(220,220,220,1)",
                pointHoverBorderWidth: 2,
                pointRadius: 1,
                pointHitRadius: 10,
                data: data,
                spanGaps: false,
            },
            {
                //凡例
                label: "注意水位",
                //面の表示
                fill: false,
                //線のカーブ
                lineTension: 0,
                //カクカクするグラフできる
                lineTension: 0.1,
                borderWidth: 1,
                //背景色
                backgroundColor: "rgba(75,192,192,0.4)",
                //枠線の色
                borderColor: "yellow",
                pointHitRadius: 3, //結合点より外でマウスホバーを認識する範囲
                pointRadius: 0,
                //グラフのデータ
                data: [350,350,350,350,350,350,350,350,350,350,350,350,350,350,350,350,350,350,350,350,350,350,350,350,350,350,350,350,350,350,350,350,350,350,350,350,350,350,350,350,350,350,350,350,350,350,350,350,350,350,350,350,350,350,350,350,350,350,350,350,350,350,350,350,350,350,350,350,350,350,350,350,350,350,350,350,350,350,350,350,350,350,350,350,350,350,350,350,350,350,350,350,350,350,350,350,350,350,350,350,350,350,350,350,350,350,350,350,350,350,350,350,350,,350,350,350,350,350,350,350,350,350,350,350,350,350,350,350,350,350,350,350,350,350,350,350,350,350,350,350,350,350,350,350,350,350,350,350,350,350,350,350,350,350,350,350,350,350,350,350,350,350,350,350,350,350,350,350,350,350,350,350,350,350,350,350,350,350,350,350,350,350,350,350,350,350,350,350,350,350,350,350,350,350,350,350,350]
            },
            {
                //凡例
                label: "危険水位",
                //面の表示
                fill: false,
                //線のカーブ
                lineTension: 0,
                //カクカクするグラフできる
                lineTension: 0.1,
                borderWidth: 1,
                //背景色
                backgroundColor: "rgba(75,192,192,0.4)",
                //枠線の色
                borderColor: "red",
                pointHitRadius: 3, //結合点より外でマウスホバーを認識する範囲
                pointRadius: 0,
                //グラフのデータ
                data: [450,450,450,450,450,450,450,450,450,450,450,450,450,450,450,450,450,450,450,450,450,450,450,450,450,450,450,450,450,450,450,450,450,450,450,450,450,450,450,450,450,450,450,450,450,450,450,450,450,450,450,450,450,450,450,450,450,450,450,450,450,450,450,450,450,450,450,450,450,450,450,450,450,450,450,450,]
            } 
        ]
    },
    //オプションの設定
    options: {
        //軸の設定
        scales: {
            //縦軸の設定
            yAxes: [{
                //目盛りの設定
                ticks: {
                    //最小値を0にする
                    beginAtZero: true,
                    min: 0,
                    max: 500
                }
            }]
        }
    }
    });
}
getCSV(); //最初に実行される