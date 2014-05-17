var deviceId = localStorage.getItem("spark_id");
var accessToken = localStorage.getItem("spark_token");

/**************** Pebble helpers ****************/

var hasKey = function(dict, key) {
  return typeof dict.payload[key] !== "undefined";
};

var getValue = function(dict, key) {
  if(hasKey(dict, key)) {
    return dict.payload[key];
  } else {
    console.log("ERROR: Key '" + key + "' does not exist in received dictionary");
    return undefined;
  }
};

/************** Configuration *******************/
Pebble.addEventListener("showConfiguration", function() {
  console.log("showing configuration");
  Pebble.openURL('http://francois-mbp.hq.getpebble.com:8000/configurable.html');
});

Pebble.addEventListener("webviewclosed", function(e) {
  console.log("configuration closed");
  // webview closed
  console.log("response : " + e.reponse);
  var options = JSON.parse(decodeURIComponent(e.response));
  accessToken = options.spark_token;
  deviceId = options.spark_id;
  console.log("id : " + deviceId);
  console.log("token : " + accessToken);
  localStorage.setItem("spark_id", deviceId);
  localStorage.setItem("spark_token", accessToken);
});

/****************** XHR *************************/

var success = function(json) {
  console.log("Success " + json.result);
  Pebble.sendAppMessage( {"SCORE": json.result} );
};

var failure = function() {
  console.log("Failure");
};

var urlForPlayer = function(player) {
  if (player > 2) {
    console.log("This is a two player game!!");
  }
  return ("https://api.spark.io/v1/devices/" + deviceId +
      "/p" + player + "?access_token=" + accessToken);
}

/****************** Main ************************/

Pebble.addEventListener("ready",
  function(e) {
    console.log("Pebble JS ready!");
  }
);

Pebble.addEventListener("appmessage",
  function(dict) {
    var url = "";
    var args = "";

    if (hasKey(dict, "UP")) {
      url = urlForPlayer(getValue(dict, "UP"));
      args = "u";
    } else if (hasKey(dict, "DOWN")) {
      url = urlForPlayer(getValue(dict, "DOWN"));
      args = "d";
    } else if (hasKey(dict, "RESET")) {
      url = "https://api.spark.io/v1/devices/" + deviceId +
      "/reset?access_token=" + accessToken;
    } else if (hasKey(dict, "SCORE")) {
      url = "https://api.spark.io/v1/devices/" + deviceId +
      "/score?access_token=" + accessToken;

      ajax({
        method: "get",
        url: url,
        type: "json"
      }, success, failure);

      return;
    } else {
      console.log("Invalid key received!");
      return;
    }

    console.log("ajax, url: " + url + ", args: " + args);

    ajax({
      method: "post",
      url: url,
      data: {"args": args},
      type: "json"
    });

  }
);
