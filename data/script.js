// Get current sensor readings when the page loads
window.addEventListener('load', getInformation);
//Function to add date and time of last update
function updateDateTime() {
    var currentdate = new Date();
    var datetime = currentdate.getDate() + "/" +
        (currentdate.getMonth() + 1) + "/" +
        currentdate.getFullYear() + " at " +
        currentdate.getHours() + ":" +
        currentdate.getMinutes() + ":" +
        currentdate.getSeconds();
    document.getElementById("update-time").innerHTML = datetime;
    console.log(datetime);
}
// Function to get current readings on the web page when it loads at first
function getReadings() {
    var xhr = new XMLHttpRequest();
    xhr.onreadystatechange = function() {
        if (this.readyState == 4 && this.status == 200) {
            var myObj = JSON.parse(this.responseText);
            console.log(myObj);
            document.getElementById("temp").innerHTML = myObj.temperature;
            document.getElementById("hum").innerHTML = myObj.humidity;
            document.getElementById("pres").innerHTML = myObj.pressure;
            updateDateTime();
        }
    };
    xhr.open("GET", "/readings", true);
    xhr.send();
}


function getInformation() {
    var xhr = new XMLHttpRequest();
    xhr.onreadystatechange = function() {
        if (this.readyState == 4 && this.status == 200) {
            var myObj = JSON.parse(this.responseText);
            console.log(myObj);
            for (i in myObj.users) {
                var name = myObj.users[i].name;
                var number = myObj.users[i].number;
                console.log(name);
                console.log(number);
                addUser(i, name, number)
            }

            for (i in myObj.messages) {
                var message = myObj.messages[i].message;
                var relay1 = myObj.messages[i].relay1;
                var relay2 = myObj.messages[i].relay2;

                console.log(message);
                console.log(relay1);
                console.log(relay2);
                addMessage(i, message, relay1, relay2)
            }
        }
    };
    xhr.open("GET", "/info", true);
    xhr.send();
}
// Create an Event Source to listen for events
if (!!window.EventSource) {
    var source = new EventSource('/events');
    source.addEventListener('open', function(e) {
        console.log("Events Connected");
    }, false);
    source.addEventListener('error', function(e) {
        if (e.target.readyState != EventSource.OPEN) {
            console.log("Events Disconnected");
        }
    }, false);
    source.addEventListener('new_readings', function(e) {
        console.log("new_readings", e.data);
        var obj = JSON.parse(e.data);
        document.getElementById("temp").innerHTML = obj.temperature;
        document.getElementById("hum").innerHTML = obj.humidity;
        document.getElementById("pres").innerHTML = obj.pressure;
        updateDateTime();
    }, false);
}

function buttonAddUser() {
    var table = document.getElementById("users");
    var id = table.rows.length - 1
    var row = table.insertRow(id);
    row.id = "user_" + id;
    var cell1 = row.insertCell(0);
    var cell2 = row.insertCell(1);
    var cell3 = row.insertCell(2);
    var cell4 = row.insertCell(3);

    cell1.id = "name_" + id;
    cell2.id = "number_" + id;
    cell1.innerHTML = "<input type=\"text\">";
    cell2.innerHTML = "<input type=\"text\">";
    cell4.innerHTML = "<button onclick=\"deleteUser(" + id + ")\"> Apagar</button>";
}

function addUser(id, name, number) {
    var table = document.getElementById("users");
    var row = table.insertRow(table.rows.length - 1);
    row.id = "user_" + id;
    var cell1 = row.insertCell(0);
    var cell2 = row.insertCell(1);
    var cell3 = row.insertCell(2);
    var cell4 = row.insertCell(3);

    cell1.innerHTML = name;
    cell1.id = "name_" + id;
    cell2.innerHTML = number;
    cell2.id = "number_" + id;
    cell3.innerHTML = "<button onclick=\"editUser(" + id + ")\">Editar</button>";
    cell4.innerHTML = "<button onclick=\"deleteUser(" + id + ")\"> Apagar</button>";

}

function editUser(user_id) {
    //name = document.getElementById("name_" + user_id).innerHTML;
    number = document.getElementById("number_" + user_id).innerHTML;
    document.getElementById("name_" + user_id).contentEditable = "true";
    // document.getElementById("name_" + user_id).innerHTML = "<input type='text' id='name_" + user_id + "' value='" + name + "'>";
    document.getElementById("number_" + user_id).innerHTML = "<input type='text' id='number_" + user_id + "' value='" + number + "'>";
}

function deleteUser(user_number) {
    document.getElementById("user_" + user_number).outerHTML = "";
}

function buttonAddMessage() {
    var table = document.getElementById("messages");
    var id = (table.rows.length - 1)
    var row = table.insertRow(id);
    row.id = "messages_" + id;
    var cell1 = row.insertCell(0);
    var cell2 = row.insertCell(1);
    var cell3 = row.insertCell(2);
    var cell4 = row.insertCell(3);
    var cell5 = row.insertCell(4);
    cell1.id = "message_" + id;
    cell2.id = "relay1_" + id;
    cell3.id = "relay2_" + id;
    cell1.innerHTML = "<input type=\"text\">";
    cell2.innerHTML = "<input type=\"text\">";
    cell2.innerHTML = "<input type=\"checkbox\">";
    cell3.innerHTML = "<input type=\"checkbox\">";
    cell5.innerHTML = "<button onclick=\"deleteString(" + id + ")\"> Apagar</button>";
}


function addMessage(id, message, relay1, relay2) {
    var table = document.getElementById("messages");
    var row = table.insertRow(table.rows.length - 1);
    row.id = "messages_" + id;
    var cell1 = row.insertCell(0);
    var cell2 = row.insertCell(1);
    var cell3 = row.insertCell(2);
    var cell4 = row.insertCell(3);
    var cell5 = row.insertCell(4);
    cell1.id = "message_" + id;
    cell2.id = "relay1_" + id;
    cell3.id = "relay2_" + id;

    cell1.innerHTML = message;

    if (relay1 == "1") {
        cell2.innerHTML = "<input type=\"checkbox\" checked>";
    } else {
        cell2.innerHTML = "<input type=\"checkbox\">";
    }
    if (relay2 == "1") {
        cell3.innerHTML = "<input type=\"checkbox\" checked>";
    } else {
        cell3.innerHTML = "<input type=\"checkbox\">";
    }
    cell4.innerHTML = "<button onclick=\"editString(" + id + ")\"> Editar</button>";
    cell5.innerHTML = "<button onclick=\"deleteString(" + id + ")\"> Apagar</button>";
}


function editString(message_id) {
    message = document.getElementById("message_" + message_id).innerHTML;
    document.getElementById("message_" + user_id).innerHTML = "<input type='text' id='message_" + message_id + "' value='" + message + "'>";
}

function deleteString(message_id) {
    document.getElementById("messages_" + message_id).outerHTML = "";
}

function saveButton() {
    var info = {
        users: [],
        messages: []
    };

    var tableUsers = document.getElementById('users');
    //gets rows of table
    var userCount = tableUsers.rows.length - 1;
    //loops through rows    
    for (i = 1; i < userCount; i++) {
        //gets cells of current row  
        var user = tableUsers.rows.item(i).cells;
        if (user.item(0).children[0] == undefined) {
            info.users.push({
                "name": user.item(0).innerText,
                "number": user.item(1).innerText,
            });
        } else {
            info.users.push({
                "name": user.item(0).children[0].value,
                "number": user.item(1).children[0].value,
            });
        }
    }

    var tableMessages = document.getElementById('messages');
    //gets rows of table
    var messagesCount = tableMessages.rows.length - 1;
    //loops through rows    
    for (i = 1; i < messagesCount; i++) {
        //gets cells of current row  
        var message = tableMessages.rows.item(i).cells;
        var relay1_message, relay2_message;
        if (message.item(1).children[0].checked) {
            relay1_message = "1"
        } else {
            relay1_message = "0"

        }
        if (message.item(2).children[0].checked) {
            relay2_message = "1"
        } else {
            relay2_message = "0"

        }


        if (message.item(0).children[0] == undefined) {
            info.messages.push({
                "message": message.item(0).innerText,
                "relay1": relay1_message,
                "relay2": relay2_message
                    //"number": user.item(1).innerText,
            });
        } else {
            info.messages.push({
                "message": message.item(0).children[0].value,
                "relay1": relay1_message,
                "relay2": relay2_message
                    //"number": user.item(1).children[0].value,
            });
        }
    }



    var myJSON = JSON.stringify(info);
    console.log(myJSON);
    alert(myJSON);
    // Sending a receiving data in JSON format using GET method
    //      
    /*  var xhr = new XMLHttpRequest();
       var url = "/update" + encodeURIComponent(myJSON);
       xhr.open("GET", url, true);
       xhr.setRequestHeader("Content-Type", "application/json");
       xhr.onreadystatechange = function() {
           if (xhr.readyState === 4 && xhr.status === 200) {
               var json = JSON.parse(xhr.responseText);
               console.log(json);
           }
       };
       xhr.send();  */

    // open request
    var xhr = new XMLHttpRequest();
    xhr.open('POST', '/update');

    // set `Content-Type` header
    xhr.setRequestHeader('Content-Type', 'application/json');

    // send rquest with JSON payload
    xhr.send(JSON.stringify(info));

}