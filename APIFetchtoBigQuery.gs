// const projectId = "testing-385318"; 
// const datasetId = "iotproject";
// function runAndPush() // Run this function
// {
//   getApiDataAndPushToBigquery()
// }

// //Get data from API
// function getApiDataAndPushToBigquery(param) { // Run Function 
//   const _api = "channels/"+_channelId+"/feeds.json";

//   var options = { 
//     method: 'GET',    
//     headers: { 
//       'content-type' : 'application/json',
//      }, 
     
//     muteHttpExceptions : true
//   };   
  
//   var result;  
//   var res='No';
  
//   try { 

//     var response = UrlFetchApp.fetch(_url  + _api +
//                                    '?api_key='+ _apiKey + '&' +
//                                    options);         
//                                    Logger.log(response)
                                   
//     if (response.getResponseCode() === 200) {    
//         result = JSON.parse(response.getContentText());
//         res = 'Yes';
//     } else {      
//       result = response.getContentText();
//     }
    
//   } catch (err) {
//     Logger.log("Error " + err);
//     result = '{ "ERROR": "' +  err + '"}';
//   } 
  
//   if (res == 'Yes') { 
//     try {
//       var data = result;
//       //Start to get data from JSON result
//       var entries = result.feeds
//       Logger.log(entries)
//       Logger.log(entries.length)
//       var channel = result.channel
//       var channelid = (channel.id).toString()
//       var channelname = channel.name
//       var channellast_entry_id = channel.last_entry_id
//       Logger.log(channelid)
//       Logger.log(channelname)
//       Logger.log(channellast_entry_id)      

//       var data = []; 

//       for (var i = 0; i < entries.length; i++) {

//         var entry_id = entries[i].entry_id;
//         var field1 = entries[i].field1;
//         var field2 = entries[i].field2;
//         var field3 = entries[i].field3;
//         var field4 = entries[i].field4;
//         var field5 = entries[i].field5;
//         var created_at = entries[i].created_at;
//         //Push data to Google Sheet
//         // pushDataToGoogleSheet([channelid,channelname,channellast_entry_id,entry_id,field1,field2,field3,field4,field5,created_at]);

//         var callPush = JSON.stringify({'channelid': parseInt(channelid),
//                                        'channelname': channelname.toString(),
//                                        'recordid': parseInt(entry_id),
//                                        'temp': parseFloat(field1),
//                                        'tds': parseFloat(field2),
//                                        'ph': parseFloat(field3),
//                                        'lat': parseFloat(field4),
//                                        'log': parseFloat(field5),
//                                       //  'createdate': created_at,
//                                       }); 
//         data.push(callPush);
       


//       } 


//        // INSERT TO Big Query
//       // var tableId = "waterquality"     
//         var tableId = "wq"     

//       var datarow = data.join("\n");  
// Logger.log(datarow)

//       var blobData = Utilities.newBlob(datarow, "application/octet-stream");
//       job = {
//         configuration: {
//           load: {
//             destinationTable: {
//               projectId: projectId,
//               datasetId: datasetId,
//               tableId: tableId
//             },
//             sourceFormat: "NEWLINE_DELIMITED_JSON",
//           }
//         }
//       }
//       Logger.log("jobs");
//       job = BigQuery.Jobs.insert(job, projectId, blobData);  
//   Logger.log(job)
   
//     } catch (err) {
//       result = '{ "ERROR": "' +  err + '"}';
//     } //end TRY CATCH   
    
//   } // if (res == 'Yes') 
  
//   return result;   
  
// } //END function 