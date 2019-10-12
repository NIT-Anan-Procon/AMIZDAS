//アイコン(図形)の追加
var icon1=L.icon({
  iconUrl: 'img/danger.png',
  iconSize:   [48,48],
  iconAnchor: [16,32],
  popupAnchor:[0,-55]
});

var icon2=L.icon({
  iconUrl: 'img/attention.png',
  iconSize:   [48,48],
  iconAnchor: [16,32],
  popupAnchor:[0,-55]
});

var icon3=L.icon({
  iconUrl: 'img/safe.png',
  iconSize:   [48,48],
  iconAnchor: [16,32],
  popupAnchor:[0,-55]
});

var icon4=L.icon({
  iconUrl: 'img/notset.png',
  iconSize:   [48,48],
  iconAnchor: [16,32],
  popupAnchor:[0,-55]
});

window.addEventListener('load', function(){
  // 地図を表示するdiv要素のidを設定
  var map = L.map('mapcontainer');
  // 地図の中心とズームレベルを指定
  map.setView([34.10,134.50], 11);
  // 表示するタイルレイヤのURLとAttributionコントロールの記述を設定して、地図に追加する
  L.tileLayer('MapData/4uMaps/{z}/{x}/{y}.png',{maxzoom: 15}).addTo(map);
  // 座標の指定、どのアイコンか指定して設置
  var marker = [];
  var $script1 = $('#script');
  var water_gage_data = JSON.parse($script1.attr('data-param1'));
  alert(water_gage_data);
  // for文でデータベースから取得した水位計の数だけ処理を繰り返す
  for (var i = 0; i < water_gage_data.length; i++) {
    let name = '水位計一号機';
    if(water_gage_data[i]["flag"]==1){
        marker[i] = L.marker([water_gage_data[i]["longitude"],water_gage_data[i]["latitude"]], {icon:   icon1}).addTo(map).bindPopup(name).openPopup();
        marker[i].on('click', onMarkerClick);
        marker[i].name = water_gage_data[i]["name"];
        marker[i].mail = "1167120@st.anan-nct.ac.jp";
    }else{
        marker[i] = L.marker([water_gage_data[i]["longitude"],water_gage_data[i]["latitude"]], {icon:   icon4}).addTo(map).bindPopup(name).openPopup();
        marker[i].on('click', onMarkerClick);
        marker[i].name = water_gage_data[i]["name"];
        marker[i].mail = "1167120@st.anan-nct.ac.jp";
    }
  }
  

  let detail = document.getElementById('scroll_btn');

  function onMarkerClick(e) {
    // マーカーのclickイベント呼び出される
    // マーカのIDをdetail.phpに送信
    console.log(e.target.name);
    $('iframe').attr('src', 'detail_u.html?name=' + e.target.name + '&mail=' + e.target.mail);

    // マーカーをクリックしたら下にスクロール
    delayExec(detail);
  }

  let top = document.getElementById('top');
  $('#scroll_btn, #scroll_btn>button').click(function(){
    scrollToElement(top);
  });

  function delayExec(element){
    setTimeout(function(){
      scrollToElement(element)
    }, 100);
  }

  function scrollToElement(element){
    element.scrollIntoView({
      behavior: 'smooth',
      block: 'start',
      inline: 'nearest'
    });
  }
});
