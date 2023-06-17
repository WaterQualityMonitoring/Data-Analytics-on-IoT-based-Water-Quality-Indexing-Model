//Push data to google sheet
function pushDataToGoogleSheet(data) {
  var sheet = SpreadsheetApp.getActiveSpreadsheet().getSheetByName("data");
    var columnNumber = 12;
    Logger.log(data)
  var dataToCheck = data[11]; 
Logger.log(dataToCheck)
  var columnValues = sheet.getRange(1, columnNumber, sheet.getLastRow(), 1).getValues();

Logger.log(columnNumber)
var f = 0;
    for (var i = 0; i < columnValues.length; i++) {
      Logger.log(columnValues[i][0])
      Logger.log(dataToCheck)
    if (columnValues[i][0] === dataToCheck) {
      f =1;
      return;
    }
  }

Logger.log(f)

if(f == 0)
{
      var lastRow = sheet.getLastRow();
  var range = sheet.getRange(lastRow + 1, 1, 1, data.length);
  range.setValues([data]);
  
}

}