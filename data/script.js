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
function getInformation() {
    var xhr = new XMLHttpRequest();
    xhr.onreadystatechange = function() {
        if (this.readyState == 4 && this.status == 200) {
            var myObj = JSON.parse(this.responseText);
            if (myObj.users.length > 0) {
                //handler either not an array or empty array
                if (myObj.users) {
                    for (i in myObj.users) {
                        var name = myObj.users[i].name;
                        var number = myObj.users[i].number;
                        addUser(i, name, number)
                    }
                }
            }
            if ((document.getElementById("users").rows.length - 2) < 10) {
                document.getElementById("rowButtonAddUser").style.display = "block";
            } else {
                document.getElementById("rowButtonAddUser").style.display = "none";
            }
            if (myObj.messages.length > 0) {
                for (i in myObj.messages) {
                    var message = myObj.messages[i].message;
                    var relay1 = myObj.messages[i].relay1;
                    var relay2 = myObj.messages[i].relay2;
                    addMessage(i, message, relay1, relay2)
                }
            }
            if ((document.getElementById("messages").rows.length - 2) < 10) {
                document.getElementById("rowButtonAddMessage").style.display = "block";
            } else {
                document.getElementById("rowButtonAddMessage").style.display = "none";
            }
        }
    };
    xhr.open("GET", "/info", true);
    xhr.send();
}


function getHistory() {
    var xhr = new XMLHttpRequest();
    xhr.onreadystatechange = function() {
        if (this.readyState == 4 && this.status == 200) {
            var myObj = JSON.parse(this.responseText);
            console.log(myObj);
            console.log("history: ", myObj.history);

            if (myObj.history.length > 0) {
                //handler either not an array or empty array

                myObj.history.reverse();
                // console.log("order: ", myObj.history);

                console.log(myObj.history);

                if (myObj.history.length > 0) {
                    //handler either not an array or empty array
                    if (myObj.history) {
                        for (i in myObj.history) {
                            var date = myObj.history[i].date;
                            var hour = myObj.history[i].hour;
                            var nameOrNumber = myObj.history[i].name;
                            var message = myObj.history[i].message;
                            addUHistory(i, date, hour, nameOrNumber, message)
                        }
                    }
                }
            }
            document.getElementById("configDiv").style.display = "none";

            document.getElementById("historyDiv").style.display = "block";


        }
    };

    xhr.open("GET", "/history", true);
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
    cell1.outerHTML = "<td class=\"users-collumn-1\"><input type=\"text\"></td>";
    cell2.outerHTML = "<td class=\"users-collumn-2\"><input type=\"text\"></td>";
    cell4.outerHTML = "<td onclick=\"deleteUser(" + id + ")\" class=\"users-collumn-4\"><img class=\"delete-icon\" src=\"delete.png\"></td>";
    if ((document.getElementById("users").rows.length - 2) < 10) {
        document.getElementById("rowButtonAddUser").style.display = "block";
    } else {
        document.getElementById("rowButtonAddUser").style.display = "none";
    }
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
    cell1.class = "users-collumn-1"
    cell2.innerHTML = number;
    cell2.id = "number_" + id;
    cell2.class = "users-collumn-2"
    cell3.outerHTML = "<td onclick=\"editUser(" + id + ")\" class=\"users-collumn-3\"><img class=\"edit-icon\" src=\"edit.png\"></td>";

    cell4.outerHTML = "<td onclick=\"deleteUser(" + id + ")\" class=\"users-collumn-4\"><img class=\"delete-icon\" src=\"delete.png\"></td>";


}


function addUHistory(i, date, hour, nameOrNumber, message) {
    var table = document.getElementById("history");
    var row = table.insertRow(table.rows.length);
    var cell1 = row.insertCell(0);
    var cell2 = row.insertCell(1);
    var cell3 = row.insertCell(2);
    var cell4 = row.insertCell(3);

    cell1.innerHTML = date;
    cell2.innerHTML = hour;
    cell3.innerHTML = nameOrNumber;
    cell4.innerHTML = message



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
    if ((document.getElementById("users").rows.length - 2) < 10) {
        document.getElementById("rowButtonAddUser").style.display = "block";
    } else {
        document.getElementById("rowButtonAddUser").style.display = "none";
    }
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
    cell1.id = "message_" + id;
    cell2.id = "relay1_" + id;
    cell3.id = "relay2_" + id;
    cell1.class = "messages-collumn-1"
    cell2.class = "messages-collumn-2";
    cell3.class = "messages-collumn-3";
    cell1.innerHTML = "<input type=\"text\">";
    cell2.innerHTML = "<input type=\"checkbox\">";
    cell3.innerHTML = "<input type=\"checkbox\">";
    cell5.outerHTML = "<td onclick=\"deleteString(" + id + ")\" class=\"messages-collumn-5\"><img class=\"delete-icon\" src=\"delete.png\"></td>";
    if ((document.getElementById("messages").rows.length - 2) < 10) {
        document.getElementById("rowButtonAddMessage").style.display = "block";
    } else {
        document.getElementById("rowButtonAddMessage").style.display = "none";
    }
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
    cell1.class = "messages-collumn-1"
    cell2.class = "messages-collumn-2";
    cell3.class = "messages-collumn-3";
    cell1.innerHTML = message;
    if (relay1 == true) {
        cell2.innerHTML = "<input type=\"checkbox\" checked>";
    } else {
        cell2.innerHTML = "<input type=\"checkbox\">";
    }
    if (relay2 == true) {
        cell3.innerHTML = "<input type=\"checkbox\" checked>";
    } else {
        cell3.innerHTML = "<input type=\"checkbox\">";
    }
    cell4.outerHTML = "<td onclick=\"editString(" + id + ")\" class=\"messages-collumn-4\"><img class=\"edit-icon\" src=\"edit.png\"></td>";

    cell5.outerHTML = "<td onclick=\"deleteString(" + id + ")\" class=\"messages-collumn-5\"><img class=\"delete-icon\" src=\"delete.png\"></td>";
}


function editString(message_id) {
    message = document.getElementById("message_" + message_id).innerHTML;
    document.getElementById("message_" + user_id).innerHTML = "<input type='text' id='message_" + message_id + "' value='" + message + "'>";
}

function deleteString(message_id) {
    document.getElementById("messages_" + message_id).outerHTML = "";
    if ((document.getElementById("messages").rows.length - 2) < 10) {
        document.getElementById("rowButtonAddMessage").style.display = "block";
    } else {
        document.getElementById("rowButtonAddMessage").style.display = "none";
    }
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
            relay1_message = true
        } else {
            relay1_message = false

        }
        if (message.item(2).children[0].checked) {
            relay2_message = true
        } else {
            relay2_message = false

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
    // open request
    var xhr = new XMLHttpRequest();
    xhr.open('POST', '/update', true);
    // set `Content-Type` header
    xhr.setRequestHeader('Content-Type', 'application/json');

    // send rquest with JSON payload
    xhr.send(JSON.stringify(info));
    var x = document.getElementsByClassName("loader");
    x[0].style.display = "block";

    setInterval('location.reload()', 5000);

}