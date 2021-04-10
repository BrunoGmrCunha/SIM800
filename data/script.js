// Get current sensor readings when the page loads
window.addEventListener('load', getInformation);
window.addEventListener('load', getSignalStrength);
//Function to add date and time of last update

// Function to get current readings on the web page when it loads at first
function getInformation() {
    var xhr = new XMLHttpRequest();
    xhr.onreadystatechange = function () {
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
            if ((document.getElementById("table-users").rows.length - 2) < 10) {
                document.getElementById("rowButtonAddUser").style.display = "flexbox";
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

            if (myObj.calls.length > 0) {
                for (i in myObj.calls) {
                    var relay1 = myObj.calls[i].relay1;
                    var relay2 = myObj.calls[i].relay2;
                    var relay1_call = document.getElementById('call_relay1').children[0];
                    var relay2_call = document.getElementById('call_relay2').children[0];


                    if (relay1 == true) {
                        relay1_call.checked = true;
                    } else {
                        relay1_call.checked = false;
                    }
                    if (relay2 == true) {
                        relay2_call.checked = true;
                    } else {
                        relay2_call.checked = false;
                    }
                }
            }
            if ((document.getElementById("table-messages").rows.length - 2) < 10) {
                document.getElementById("rowButtonAddMessage").style.display = "flexbox";
            } else {
                document.getElementById("rowButtonAddMessage").style.display = "none";
            }
        }
    };
    xhr.open("GET", "/config", true);
    xhr.send();
}




function history() {
    document.getElementsByClassName("active")[0].classList.remove("active");
    document.getElementById("history-nav").classList.add("active");
    document.getElementsByClassName("save-icon")[0].style.display = "none";
    document.getElementsByClassName("div-configurations")[0].style.display = "none";
    document.getElementsByClassName("div-history")[0].style.display = "block";
    var xhr = new XMLHttpRequest();
    xhr.onreadystatechange = function () {
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
            /*  document.getElementsByClassName("active")[0].classList.remove("active");
             document.getElementById("history-nav").className=("active");
             document.getElementsByClassName("div-configurations")[0].style.display = "none";
             document.getElementsByClassName("div-history")[0].style.display = "block"; */
        }
    };

    xhr.open("GET", "/history", true);
    xhr.send();

}

function getSignalStrength() {
    var xhr = new XMLHttpRequest();
    xhr.onreadystatechange = function () {
        if (this.readyState == 4 && this.status == 200) {
            var myObj = JSON.parse(this.responseText);
            console.log(myObj);
            console.log("SignalStrength: ", myObj.SignalStrength);
            var signalStrengthSpan = document.getElementById('signal-strength');

            if (myObj.SignalStrength < 2) {
                signalStrengthSpan.innerText = "Sem Sinal";
            }
            else if (myObj.SignalStrength <= 2 && myObj.SignalStrength < 10) {
                signalStrengthSpan.innerText = "Sinal: Fraco";
            }
            else if (myObj.SignalStrength >= 10 && myObj.SignalStrength < 15) {
                signalStrengthSpan.innerText = "Sinal: Médio";
            }
            else if (myObj.SignalStrength >= 15 && myObj.SignalStrength < 20) {
                signalStrengthSpan.innerText = "Sinal: Bom";
            }
            else if (myObj.SignalStrength >= 20 && myObj.SignalStrength <= 30) {
                signalStrengthSpan.innerText = "Sinal: Muito Bom";
            }

            /*  document.getElementsByClassName("active")[0].classList.remove("active");
             document.getElementById("history-nav").className=("active");
             document.getElementsByClassName("div-configurations")[0].style.display = "none";
             document.getElementsByClassName("div-history")[0].style.display = "block"; */
        }
    };

    xhr.open("GET", "/signalStrength", true);
    xhr.send();

}



function configuration() {
    document.getElementsByClassName("active")[0].classList.remove("active");
    document.getElementById("configuration-nav").classList.add("active");
    document.getElementsByClassName("save-icon")[0].style.display = "block";
    document.getElementsByClassName("div-configurations")[0].style.display = "block";
    document.getElementsByClassName("div-history")[0].style.display = "none";
}
// Create an Event Source to listen for events
if (!!window.EventSource) {
    var source = new EventSource('/events');
    source.addEventListener('open', function (e) {
        console.log("Events Connected");
    }, false);
    source.addEventListener('error', function (e) {
        if (e.target.readyState != EventSource.OPEN) {
            console.log("Events Disconnected");
        }
    }, false);
    source.addEventListener('new_readings', function (e) {
        console.log("new_readings", e.data);
        var obj = JSON.parse(e.data);
        document.getElementById("temp").innerHTML = obj.temperature;
        document.getElementById("hum").innerHTML = obj.humidity;
        document.getElementById("pres").innerHTML = obj.pressure;
        updateDateTime();
    }, false);
}

function buttonAddUser() {
    var table = document.getElementById("table-users");
    var id = table.rows.length - 1
    var row = table.insertRow(id);
    row.id = "user_" + id;
    var nameCell = row.insertCell(0);
    var numberCell = row.insertCell(1);
    var deleteCell = row.insertCell(2);
    var editCell = row.insertCell(3);
    nameCell.id = "name_" + id;
    numberCell.id = "number_" + id;
    nameCell.innerHTML = "<input type=\"text\" placeholder=\"nome\">";
    numberCell.innerHTML = "<input type=\"text\" placeholder=\"número\" pattern=\"[0-9]{9}\">";
    editCell.outerHTML = "<td></td>";
    deleteCell.outerHTML = "<td onclick=\"deleteUser(" + id + ")\"><img height=\"15px\"src=\"images/trash-alt-regular.svg\"></td>";
    if ((table.rows.length - 1) < 10) {
        document.getElementById("rowButtonAddUser").style.display = "flexbox";
    } else {
        document.getElementById("rowButtonAddUser").style.display = "none";
    }
}

function addUser(id, name, number) {
    var table = document.getElementById("table-users");
    var row = table.insertRow(table.rows.length - 1);
    row.id = "user_" + id;
    var nameCell = row.insertCell(0);
    var numberCell = row.insertCell(1);
    var editCell = row.insertCell(2);
    var deleteCell = row.insertCell(3);
    nameCell.id = "name_" + id;
    numberCell.id = "number_" + id;
    nameCell.innerHTML = name;
    numberCell.innerHTML = number;
    editCell.outerHTML = "<td onclick=\"editUser(" + id + ")\"><img height=\"15px\"src=\"images/edit-regular.svg\"></td>";
    deleteCell.outerHTML = "<td onclick=\"deleteUser(" + id + ")\"><img height=\"15px\"src=\"images/trash-alt-regular.svg\"></td>";
}


function addUHistory(i, date, hour, nameOrNumber, message) {
    var table = document.getElementById("table-history");
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
    nameContent = document.getElementById("name_" + user_id).innerHTML;
    numberContent = document.getElementById("number_" + user_id).textContent;
    document.getElementById("name_" + user_id).innerHTML = "<input type='text' id='name_" + user_id + "' value='" + nameContent + "'>";
    document.getElementById("number_" + user_id).innerHTML = "<input type='text' id='number_" + user_id + "' value='" + numberContent + "'>";
}

function deleteUser(user_number) {
    document.getElementById("user_" + user_number).outerHTML = "";
    if ((document.getElementById("table-users").rows.length - 2) < 10) {
        document.getElementById("rowButtonAddUser").style.display = "flexbox";
    } else {
        document.getElementById("rowButtonAddUser").style.display = "none";
    }
}

function buttonAddMessage() {
    var table = document.getElementById("table-messages");
    var id = (table.rows.length - 1)
    var row = table.insertRow(id);
    row.id = "messages_" + id;
    var messageCell = row.insertCell(0);
    var relay1Cell = row.insertCell(1);
    var relay2Cell = row.insertCell(2);
    var deleteCell = row.insertCell(3);
    var editCell = row.insertCell(4);
    messageCell.id = "message_" + id;
    relay1Cell.id = "relay1_" + id;
    relay2Cell.id = "relay2_" + id;

    messageCell.innerHTML = "<input type=\"text\">";
    relay1Cell.innerHTML = "<input type=\"checkbox\">";
    relay2Cell.innerHTML = "<input type=\"checkbox\">";
    deleteCell.outerHTML = "<td onclick=\"deleteMessage(" + id + ")\"><img height=\"15px\"src=\"images/trash-alt-regular.svg\"></td>";
    editCell.outerHTML = "<td> </td>"
    if ((document.getElementById("table-messages").rows.length - 2) < 10) {
        document.getElementById("rowButtonAddMessage").style.display = "flexbox";
    } else {
        document.getElementById("rowButtonAddMessage").style.display = "none";
    }
}


function addMessage(id, message, relay1, relay2) {
    var table = document.getElementById("table-messages");
    var row = table.insertRow(table.rows.length - 1);
    row.id = "messages_" + id;
    var messageCell = row.insertCell(0);
    var relay1Cell = row.insertCell(1);
    var relay2Cell = row.insertCell(2);
    var editCell = row.insertCell(3);
    var deleteCell = row.insertCell(4);
    messageCell.id = "message_" + id;
    relay1Cell.id = "relay1_" + id;
    relay2Cell.id = "relay2_" + id;
    messageCell.innerHTML = message;
    if (relay1 == true) {
        relay1Cell.innerHTML = "<input type=\"checkbox\" checked disabled=\"true\">";
    } else {
        relay1Cell.innerHTML = "<input type=\"checkbox\" disabled=\"true\">";
    }
    if (relay2 == true) {
        relay2Cell.innerHTML = "<input type=\"checkbox\" checked disabled=\"true\">";
    } else {
        relay2Cell.innerHTML = "<input type=\"checkbox\" disabled=\"true\">";
    }
    editCell.outerHTML = "<td onclick=\"editMessage(" + id + ")\"><img height=\"15px\"src=\"images/edit-regular.svg\"></td>";
    deleteCell.outerHTML = "<td onclick=\"deleteMessage(" + id + ")\"><img height=\"15px\"src=\"images/trash-alt-regular.svg\"></td>";
}


function editMessage(message_id) {
    message = document.getElementById("message_" + message_id).innerHTML;
    document.getElementById("relay1_" + message_id).firstChild.disabled = false;
    document.getElementById("relay2_" + message_id).firstChild.disabled = false;
    document.getElementById("message_" + message_id).innerHTML = "<input type='text' id='message_" + message_id + "' value='" + message + "'>";
}

function deleteMessage(message_id) {
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
        messages: [],
        calls: []
    };

    var tableUsers = document.getElementById('table-users');
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

    var tableMessages = document.getElementById('table-messages');
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
    var relay1_call, relay2_call;

    if (document.getElementById('call_relay1').children[0].checked) {
        relay1_call = true;
    } else {
        relay1_call = false;

    }
    if (document.getElementById('call_relay2').children[0].checked) {
        relay2_call = true;
    } else {
        relay2_call = false;

    }


    info.calls.push({
        "relay1": relay1_call,
        "relay2": relay2_call
        //"number": user.item(1).children[0].value,
    });

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