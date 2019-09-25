//アイコン(図形)の追加
var icon1=L.icon({
  iconUrl: 'img/danger.png',
  iconSize:   [32,32],
  iconAnchor: [16,32],
  popupAnchor:[0,-55]
});

var icon2=L.icon({
  iconUrl: 'img/attention.png',
  iconSize:   [32,32],
  iconAnchor: [16,32],
  popupAnchor:[0,-55]
});

var icon3=L.icon({
  iconUrl: 'img/safe.png',
  iconSize:   [32,32],
  iconAnchor: [16,32],
  popupAnchor:[0,-55]
});


window.addEventListener('load', function(){
  //地図を表示するdiv要素のidを設定
  var map = L.map('mapcontainer');
  //地図の中心とズームレベルを指定
  map.setView([34.10,134.50], 11);
  //表示するタイルレイヤのURLとAttributionコントロールの記述を設定して、地図に追加する
  L.tileLayer('アトラス_2019-08-20_155246/4uMaps/{z}/{x}/{y}.png',{maxzoom: 15}).addTo(map);
  //座標の指定、どのアイコンか指定して設置
  L.marker([34.10,134.50], {icon: icon1}).addTo(map).bindPopup('水位計1号機').openPopup();
})

window.addEventListener('load', function(){
  //地図を表示するdiv要素のidを設定
  var map = L.map('mapcontainer2');
  //地図の中心とズームレベルを指定
  map.setView([34.05,134.43], 11);
  //表示するタイルレイヤのURLとAttributionコントロールの記述を設定して、地図に追加する
  L.tileLayer('アトラス_2019-08-20_155246/4uMaps/{z}/{x}/{y}.png',{maxzoom: 15}).addTo(map);
  //座標の指定、どのアイコンか指定して設置
  L.marker([34.05,134.43], {icon: icon2}).addTo(map).bindPopup('水位計2号機').openPopup();
})