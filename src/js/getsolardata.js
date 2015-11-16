var SolaruserID = 'Something like this f24twer-2346-hdfg-i38gf-eragaerfq3gs43';
var SolarDate = '2015-10-09';
var solar_HourPower = [];
var myJsonString;

var xhrRequest = function (url, type, callback) {
  var xhr = new XMLHttpRequest();
  xhr.onload = function () {
    callback(this.responseText);
  };
  xhr.open(type, url);
  xhr.send();
};

function getSolarData() {
  console.log("Function: getSolarData");
  // Construct URL
  var url = 'http://www.goodwe-power.com/Mobile/GetMyPowerStationById?stationID=' + SolaruserID;
   // Sending complete URL for debug
  console.log('getSolarData URL = ' + url);
  xhrRequest(url, 'GET', 
    function(responseText) {
      // responseText contains a JSON object with weather info
      var json = JSON.parse(responseText);

      // Temperature in Kelvin requires adjustment
      var stat_curpower = json.curpower;
      console.log("curpower :" + stat_curpower);

      // Conditions
      var stat_eday = json.eday;      
      console.log("eday     : " + stat_eday);
      
      // Assemble dictionary using our keys
      var dictionary = {
        "KEY_CURPOWER": stat_curpower,
        "KEY_EDAY": stat_eday
      };

      // Send to Pebble
      console.log("Sent dictionary for: getSolarData");
      Pebble.sendAppMessage(dictionary,
        function(e) {
          console.log("Solar Data sent to Pebble successfully!");
        },
        function(e) {
          console.log("Error sending solar data to Pebble!");
        }
      );
    }      
  );
}


function getSolarValues(date) {
   console.log("Function: getSolarValues");
  // Construct URL
  var url = 'http://www.goodwe-power.com/Mobile/GetPacLineChart?stationId=' + SolaruserID + '&date=' + date;
   // Sending complete URL for debug
  console.log('getSolarValues URL = ' + url);
  
  // Send request to URL
    xhrRequest(url, 'GET', 
      function(responseText) {
      // responseText contains JSON data
      // solar_json contains java objects
      var solar_json = JSON.parse(responseText);
      var solar_json_count = Object.keys(solar_json).length;

        // importing json data to array[]
        for ( var counter = 0; counter < solar_json_count; counter++ ) {
        solar_HourPower[counter] = Number(solar_json[counter].HourPower);
        }
        
//        for ( counter = 0; counter < Object.keys(solar_json).length; counter++ ) {
//          console.log('Results = ' + solar_HourPower[counter]);          
//        }

        // create json from array:
        myJsonString = JSON.stringify(solar_HourPower);
        console.log('myJsonString = ' + myJsonString);
        
             // Assemble dictionary using our keys
      var dictionary = {
        "KEY_SOLARVALUE": myJsonString
      };

      // Send to Pebble
      console.log("Sent dictionary for: getSolarValues");
      Pebble.sendAppMessage(dictionary,
        function(e) {
          console.log("Solar values sent to Pebble successfully!");
        },
        function(e) {
          console.log("Error sending solar values to Pebble!");
        }  
      ); 
        
        
      }
    );
}      



// Listen for when the watchface is opened
Pebble.addEventListener('ready', 
  function(e) {
    console.log("PebbleKit JS ready!");

    // Get the solar data
    getSolarData();
    getSolarValues(SolarDate);
  }
);

// Listen for when an AppMessage is received
Pebble.addEventListener('appmessage',
  function(e) {
    console.log("AppMessage received!");
    getSolarData();
  }                     
);
