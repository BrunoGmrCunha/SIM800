var _configuration;

var divIndex = document.getElementById('index');
var divUsers = document.getElementById('users');
var divMessages = document.getElementById('messages');
var divCalls = document.getElementById('calls');
var divSettings = document.getElementById('settings');



//general
function back() {
    if ((divSettings.getElementsByClassName('container-settings-sim-card')[0].style.display == "block") || (divSettings.getElementsByClassName('container-settings-wifi')[0].style.display == "block") || (divSettings.getElementsByClassName('container-settings-doors-name')[0].style.display == "block")) {
        divSettings.getElementsByClassName('container-page-title')[0].style.display = "block";
        divSettings.getElementsByClassName('container-settings-menu')[0].style.display = "block";
        divSettings.getElementsByClassName('container-settings-sim-card')[0].style.display = "none";
        divSettings.getElementsByClassName('container-settings-wifi')[0].style.display = "none";
        divSettings.getElementsByClassName('container-settings-doors-name')[0].style.display = "none";

    }
    else {
        window.scrollTo(0, 0);
        divIndex.style.display = "block";
        divUsers.style.display = "none";
        divMessages.style.display = "none";
        divCalls.style.display = "none";
        divSettings.style.display = "none";
        document.getElementsByClassName("container-back")[0].style.display = "none";
        countersUpdate();
    }
}

//Index
function fireBehaviorMenuList() {
    const menuItems = document.getElementsByClassName('menu-item')
    for (let index = 0; index < menuItems.length; index++) {
        const element = menuItems[index];
        element.addEventListener('click', function () {
            menuRedirect(this.id);
        })

    }
}

function menuRedirect(id) {
    switch (id) {
        case 'users-index':
            window.scrollTo(0, 0);

            document.getElementById("index").style.display = "none";
            document.getElementById("users").style.display = "block";
            document.getElementsByClassName("container-back")[0].style.display = "flex";
            loadUsers();

            break;
        case 'messages-index':
            window.scrollTo(0, 0);
            document.getElementById("index").style.display = "none";
            document.getElementById("messages").style.display = "block";
            document.getElementsByClassName("container-back")[0].style.display = "flex";
            loadMessages();
            break;
        case 'calls-index':
            window.scrollTo(0, 0);
            document.getElementById("index").style.display = "none";
            document.getElementById("calls").style.display = "block";
            document.getElementsByClassName("container-back")[0].style.display = "flex";
            loadCalls();
            break;
        case 'settings-index':
            window.scrollTo(0, 0);
            window.location = 'settings.html';
            break;
        default:
            break;
    }
}

function countersUpdate() {
    if (_configuration === null) {
        _configuration = "{\"users\":[],\"messages\":[],\"door1\":\"Portão Fora\",\"door2\":\"Portão Dentro\",\"calls\":[]}";
        localStorage.setItem('Configuration', _configuration);
    }
    console.log(_configuration);
    let usersCount = document.getElementById('users-count');
    let messagesCount = document.getElementById('messages-count');
    let callsCount = document.getElementById('calls-count')

    let users = [..._configuration.users];
    if (users === undefined) {
        usersCount.innerText = 0;
    }
    else {
        usersCount.innerText = users.length;
    }

    let messages = [..._configuration.messages];
    if (messages === undefined) {
        messagesCount.innerText = 0;
    } else {
        messagesCount.innerText = messages.length;
    }

    let calls = [..._configuration.calls];
    if (calls === undefined) {
        callsCount.innerText = 0;
    }
    else {
        callsCount.innerText = calls.length;

    }
}

async function getSignalStrength() {
    let signalStrength;
    await fetch('signalStrength').then(function (response) {
        var contentType = response.headers.get("content-type");
        if (contentType && contentType.indexOf("application/json") !== -1) {
            return response.json().then(function (json) {
                signalStrength = json.SignalStrength;
                const signalStrengthImg = document.getElementsByClassName('icon-signal')[0];
                console.log("signal :", json.SignalStrength);
                if (json.SignalStrength < 2) {
                    console.log("0");
                    signalStrengthImg.src = 'img/rede_0.svg'
                }
                else if (json.SignalStrength >= 2 && json.SignalStrength < 10) {
                    console.log("1");

                    signalStrengthImg.src = 'img/rede_1.svg'
                }
                else if (json.SignalStrength >= 10 && json.SignalStrength < 15) {
                    console.log("2");

                    signalStrengthImg.src = 'img/rede_2.svg'
                }
                else if (json.SignalStrength >= 15 && json.SignalStrength < 20) {
                    console.log("3");

                    signalStrengthImg.src = 'img/rede_3.svg'
                }
                else if (json.SignalStrength >= 20 && json.SignalStrength <= 30) {
                    console.log("4");
                    signalStrengthImg.src = 'img/rede_4.svg'
                }

            });
        } else {
            console.log("Oops, we haven't got JSON!");
            signalStrength = 0;
        }
    });
    localStorage.setItem('SignalStrength', signalStrength);

}

function removeOptions(selectElement) {
    var length = selectElement.options.length;
    for (i = length - 1; i >= 0; i--) {
        selectElement.options[i] = null;
    }
}

//Users
function loadUsers() {
    //_configuration = JSON.parse(localStorage.getItem('Configuration'));
    const btnAddUser = divUsers.getElementsByClassName('container-add-new')[0];
    const btnSaveUser = divUsers.getElementsByClassName('save-button')[0];


    if (_configuration === null)
        return;
    let users = [..._configuration.users];
    if (users === undefined)
        return;

    const divUserCards = divUsers.getElementsByClassName('container-card-insert-user');
    const divUserCardsLength = divUserCards.length;
    for (let index = 0; index < divUserCardsLength; index++) {
        const userCard = divUserCards[0];
        userCard.remove();
    }

    for (let index = 0; index < users.length; index++) {
        const user = users[index];
        var divModel = divUsers.getElementsByClassName('container-card-insert-user-model')[0];
        var divNewUser = divModel.cloneNode(true);
        divNewUser.className = 'container-card-insert-user';
        divNewUser.style.display = 'block'
        btnAddUser.parentNode.insertBefore(divNewUser, btnAddUser);

        let btnDelete = divNewUser.getElementsByClassName('icon-trash')[0]
        btnDelete.addEventListener('click', function () {
            this.parentNode.parentNode.parentNode.removeChild(this.parentNode.parentNode);
            btnSaveUser.disabled = false;
        })
        let nameField = divNewUser.getElementsByClassName('name-input')[0];
        nameField.value = user.name;
        nameField.addEventListener('change', function () {
            validadeteSaveButton();
        })

        let numberField = divNewUser.getElementsByClassName('number-input')[0];
        numberField.value = user.number;
        numberField.addEventListener('change', function () {
            validadeteSaveButton();
        });
    }
}

function validadeteSaveButton() {
    let saveBtn = divUsers.getElementsByClassName('save-button')[0];

    var cards = divUsers.getElementsByClassName('container-card-insert-user');
    for (let index = 0; index < cards.length; index++) {
        const card = cards[index];

        let nameField = card.getElementsByClassName('name-input')[0];
        let validateName = validateFields(nameField);
        let numberField = card.getElementsByClassName('number-input')[0];
        let validateNumber = validateFields(numberField);

        if (!(validateName && validateNumber)) {
            saveBtn.disabled = true;
            return true;
        }

    }
    saveBtn.disabled = false;
}

function validateFields(inputFiel) {
    if (inputFiel.type === "text") {
        if (inputFiel.value.length < 3) {
            inputFiel.style.borderColor = "red";
            return false;
        }
        else {
            inputFiel.style.borderColor = "#D8D8D8";
        }
    }
    else if (inputFiel.type === "tel") {
        if (inputFiel.value.length != 9) {
            inputFiel.style.borderColor = "red";
            return false;

        }
        else {
            inputFiel.style.borderColor = "#D8D8D8";
        }
    }
    return true;
}

function addNewUserBtn() {
    const btnAddUser = divUsers.getElementsByClassName('container-add-new')[0];
    const btnSaveUser = divUsers.getElementsByClassName('save-button')[0];

    btnAddUser.addEventListener('click', function () {
        var divUserModel = divUsers.getElementsByClassName('container-card-insert-user-model')[0];
        var divNewUser = divUserModel.cloneNode(true);
        divNewUser.style.display = 'block'
        divNewUser.className = 'container-card-insert-user';
        btnAddUser.parentNode.insertBefore(divNewUser, btnAddUser);

        let btnDeleteUser = divNewUser.getElementsByClassName('icon-trash')[0]
        btnDeleteUser.addEventListener('click', function () {
            this.parentNode.parentNode.parentNode.removeChild(this.parentNode.parentNode);
            btnSaveUser.disabled = false;
        })
        let nameField = divNewUser.getElementsByClassName('name-input')[0];
        nameField.addEventListener('change', function () {
            validadeteSaveButton();
        })

        let numberField = divNewUser.getElementsByClassName('number-input')[0];

        numberField.addEventListener('change', function () {
            validadeteSaveButton();
        });
    });
}

async function saveUserBtn() {
    const btnSaveUser = divUsers.getElementsByClassName('save-button')[0];

    btnSaveUser.addEventListener('click', async function () {
        let users = [];
        var cards = document.getElementsByClassName('container-card-insert-user');
        for (let index = 0; index < cards.length; index++) {
            const card = cards[index];
            let nameField = card.getElementsByClassName('name-input')[0];
            let numberField = card.getElementsByClassName('number-input')[0];
            let user = {};
            user.name = nameField.value;
            user.number = numberField.value;
            users.push(user);
        }
        console.log(users);

        _configuration.users = [...users];
        console.log("configuration");
        console.log(_configuration);
        let message = document.getElementsByClassName('container-message')[0];
        message.style.display = "flex";
        setInterval(function () {
            message.style.display = "none";
        }, 2000)


        localStorage.setItem('Configuration', JSON.stringify(_configuration));
        btnSaveUser.disabled = true;
        let myHeaders = new Headers();
        myHeaders = {
            method: 'POST',
            body: JSON.stringify(_configuration)
        };
        await fetch("configuration", myHeaders)
            .then(response => {
                console.log(response);

                const contentType = response.headers.get('content-type');
                if (!contentType || !contentType.includes('application/json')) {
                    throw new TypeError("Oops, we haven't got JSON!");
                }
                return response.json();
            })
            .then(data => {
                console.log(data);
            })
            .catch(error => console.error(error));
    })

}


//Messages
function loadMessages() {
    _configuration = JSON.parse(localStorage.getItem('Configuration'));
    const btnAddMessage = divMessages.getElementsByClassName('container-add-new')[0];
    const btnSaveMessages = divMessages.getElementsByClassName('save-button')[0];

    const divMessagesCards = divMessages.getElementsByClassName('container-card-insert-message');
    const divMessagesCardsLength = divMessagesCards.length;
    for (let index = 0; index < divMessagesCardsLength; index++) {
        const messageCard = divMessagesCards[0];
        messageCard.remove();
    }


    if (_configuration === null) {
        divMessages.getElementsByClassName('container-message-no-users')[0].style.display = 'flex';
        divMessages.getElementsByClassName('container-save-button')[0].style.display = 'none';
        btnAddMessage.style.display = 'none';
        return;
    }


    _users = [..._configuration.users];
    if (_users === undefined || _users.length <= 0) {
        divMessages.getElementsByClassName('container-message-no-users')[0].style.display = 'flex';
        divMessages.getElementsByClassName('container-save-button')[0].style.display = 'none';
        btnAddMessage.style.display = 'none';
        return;
    }

    divMessages.getElementsByClassName('container-message-no-users')[0].style.display = 'none';
    divMessages.getElementsByClassName('container-save-button')[0].style.display = 'block';
    btnAddMessage.style.display = 'block';

    const divMessagesModel = divMessages.getElementsByClassName('container-card-insert-message-model')[0];

    var listingUsers = divMessagesModel.getElementsByTagName('select')[0];
    var listingDoors = divMessagesModel.getElementsByTagName('select')[1];


    removeOptions(listingUsers);

    removeOptions(listingDoors);

    for (let index = 0; index < _users.length; index++) {
        const user = _users[index];
        option = document.createElement('option');
        option.value = user.number;
        option.text = user.name;
        listingUsers.add(option);
    }


    option = document.createElement('option');
    option.value = "door1";
    option.text = _configuration.door1;
    listingDoors.add(option);

    option = document.createElement('option');
    option.value = "door2";
    option.text = _configuration.door2;
    listingDoors.add(option);



    _messages = [_configuration.messages];

    if (_messages === undefined)
        return;

    for (let index = 0; index < _messages.length; index++) {
        const message = _messages[index];
        var divNewMessage = divMessagesModel.cloneNode(true);
        divNewMessage.style.display = 'block'
        divNewMessage.className = 'container-card-insert-message'
        btnAddMessage.parentNode.insertBefore(divNewMessage, btnAddMessage);

        let btnDeleteMessage = divNewMessage.getElementsByClassName('icon-trash')[0]
        btnDeleteMessage.addEventListener('click', function () {
            this.parentNode.parentNode.parentNode.removeChild(this.parentNode.parentNode);
            btnSaveMessages.disabled = false;
        })

        let listingUsers = divNewMessage.getElementsByTagName('select')[0];
        let listingDoors = divNewMessage.getElementsByTagName('select')[1];
        let users = message.numbers;

        for (let index = 0; index < users.length; index++) {
            const user = users[index];
            for (let index = 0; index < listingUsers.length; index++) {
                const element = listingUsers[index];
                if (element.value === user) {
                    element.selected = true;
                }
            }
        }
        if (message.door1) {
            for (let index = 0; index < listingDoors.length; index++) {
                const element = listingDoors[index];
                if (element.value === 'door1')
                    element.selected = true;

            }
        }
        if (message.door2) {
            for (let index = 0; index < listingDoors.length; index++) {
                const element = listingDoors[index];
                if (element.value === 'door2')
                    element.selected = true;
            }
        }
        let messageField = divNewMessage.getElementsByClassName('message-input')[0];
        messageField.value = message.message;
        messageField.addEventListener('change', function () {
            validadeteSaveMessagesButton();
        })

        listingUsers.addEventListener('change', function () {
            validadeteSaveMessagesButton();
        })
        listingDoors.addEventListener('change', function () {
            validadeteSaveMessagesButton();
        })
    }
}

function validadeteSaveMessagesButton() {
    const btnSaveMessages = divMessages.getElementsByClassName('save-button')[0];

    var cards = divMessages.getElementsByClassName('container-card-insert-message');
    for (let index = 0; index < cards.length; index++) {
        const card = cards[index];
        let listingUsers = card.getElementsByTagName('select')[0];
        let validateUsers = false;
        if (listingUsers.selectedOptions.length > 0)
            validateUsers = true;


        let listingDoors = card.getElementsByTagName('select')[1];
        let validateDoors = false;

        if (listingDoors.selectedOptions.length > 0)
            validateDoors = true;

        let messageField = card.getElementsByClassName('message-input')[0];

        let validateMessage = validateFields(messageField);

        if (!(validateUsers && validateDoors && validateMessage)) {
            btnSaveMessages.disabled = true;
            return true;
        }

    }
    btnSaveMessages.disabled = false;
}

function addNewMessageBtn() {
    const btnAddMessage = divMessages.getElementsByClassName('container-add-new')[0];
    const btnSaveMessage = divMessages.getElementsByClassName('save-button')[0];

    btnAddMessage.addEventListener('click', function () {
        const divMessagesModel = divMessages.getElementsByClassName('container-card-insert-message-model')[0];

        var divNewMessage = divMessagesModel.cloneNode(true);
        divNewMessage.style.display = 'block'
        divNewMessage.className = 'container-card-insert-message';
        btnAddMessage.parentNode.insertBefore(divNewMessage, btnAddMessage);

        let btnDeleteMessage = divNewMessage.getElementsByClassName('icon-trash')[0]
        btnDeleteMessage.addEventListener('click', function () {
            this.parentNode.parentNode.parentNode.removeChild(this.parentNode.parentNode);
            btnSaveMessage.disabled = false;
        })

        let listingUsers = divNewMessage.getElementsByTagName('select')[0];
        let listingDoors = divNewMessage.getElementsByTagName('select')[1];
        let messageField = divNewMessage.getElementsByClassName('message-input')[0];
        messageField.addEventListener('change', function () {
            validadeteSaveMessagesButton();
        })

        listingUsers.addEventListener('change', function () {
            validadeteSaveMessagesButton();
        })
        listingDoors.addEventListener('change', function () {
            validadeteSaveMessagesButton();
        })
    });
}

async function saveMessagesBtn() {
    const btnSaveMessage = divMessages.getElementsByClassName('save-button')[0];

    btnSaveMessage.addEventListener('click', async function () {
        let messages = [];
        var cards = divMessages.getElementsByClassName('container-card-insert-message');
        for (let index = 0; index < cards.length; index++) {
            const card = cards[index];
            let messageField = card.getElementsByClassName('message-input')[0];
            let listingUsers = card.getElementsByTagName('select')[0];
            let numbers = [];
            let door1, door2 = false;
            let listingDoors = card.getElementsByTagName('select')[1];
            for (let index = 0; index < listingUsers.selectedOptions.length; index++) {
                const element = listingUsers.selectedOptions[index];
                numbers.push(element.value);
            }
            for (let index = 0; index < listingDoors.selectedOptions.length; index++) {
                const element = listingDoors.selectedOptions[index];
                if (element.value === "door1") {
                    door1 = true;
                }
                if (element.value === "door2") {
                    door2 = true;
                }
            }
            let message = {};
            message.message = messageField.value;
            message.numbers = numbers;
            message.door1 = door1;
            message.door2 = door2;
            messages.push(message);
        }
        console.log(messages);
        let messageDisplay = divMessages.getElementsByClassName('container-message')[0];
        messageDisplay.style.display = "flex";

        setInterval(function () {
            messageDisplay.style.display = "none";
        }, 2000)
        _configuration.messages = [...messages];
        localStorage.setItem('Configuration', JSON.stringify(_configuration));
        btnSaveMessage.disabled = true;
        await fetch("configuration",
            {
                method: "POST",
                body: JSON.stringify(_configuration)
            })
            .then(function (res) { return res.json(); })
            .then(function (data) { log(data) });
    })

}

//Calls
function loadCalls() {
    debugger;
    _configuration = JSON.parse(localStorage.getItem('Configuration'));
    const btnAddCall = divCalls.getElementsByClassName('container-add-new')[0];
    const btnSaveCalls = divCalls.getElementsByClassName('save-button')[0];

    const divCallsCards = divCalls.getElementsByClassName('container-card-insert-call');
    const divCallsCardsLength = divCallsCards.length;
    for (let index = 0; index < divCallsCardsLength; index++) {
        const callCard = divCallsCards[0];
        callCard.remove();
    }

    if (_configuration === null) {
        divCalls.getElementsByClassName('container-message-no-users')[0].style.display = 'flex';
        divCalls.getElementsByClassName('container-save-button')[0].style.display = 'none';
        btnAddCall.style.display = 'none';
        return;
    }
    _users = [_configuration.users];
    if (_users === undefined || _users.length <= 0) {
        divCalls.getElementsByClassName('container-message-no-users')[0].style.display = 'flex';
        divCalls.getElementsByClassName('container-save-button')[0].style.display = 'none';
        btnAddCall.style.display = 'none';
        return;
    }
    divCalls.getElementsByClassName('container-message-no-users')[0].style.display = 'none';
    divCalls.getElementsByClassName('container-save-button')[0].style.display = 'block';
    btnAddCall.style.display = 'block';
    const divCallsModel = divCalls.getElementsByClassName('container-card-insert-call-model')[0];

    let listingUsers = divCallsModel.getElementsByTagName('select')[0];
    let listingDoors = divCallsModel.getElementsByTagName('select')[1];

    removeOptions(listingUsers);
    removeOptions(listingDoors);

    for (let index = 0; index < _users.length; index++) {
        const user = _users[index];
        option = document.createElement('option');
        option.value = user.number;
        option.text = user.name;
        listingUsers.add(option);
    }


    option = document.createElement('option');
    option.value = "door1";
    option.text = _configuration.door1;
    listingDoors.add(option);

    option = document.createElement('option');
    option.value = "door2";
    option.text = _configuration.door2;
    listingDoors.add(option);
    _calls = [..._configuration.calls];
    if (_configuration === null)
        return;
    if (_calls === undefined)
        return;


    for (let index = 0; index < _calls.length; index++) {
        const call = _calls[index];
        var divNewCall = divCallsModel.cloneNode(true);
        divNewCall.style.display = 'block'
        divNewCall.className = 'container-card-insert-call';
        btnAddCall.parentNode.insertBefore(divNewCall, btnAddCall);

        let btnDelete = divNewCall.getElementsByClassName('icon-trash')[0]
        btnDelete.addEventListener('click', function () {
            this.parentNode.parentNode.parentNode.removeChild(this.parentNode.parentNode);
            btnSaveCalls.disabled = false;
        })

        let listingUsers = divNewCall.getElementsByTagName('select')[0];
        let listingDoors = divNewCall.getElementsByTagName('select')[1];
        let users = _calls[index].numbers;

        for (let index = 0; index < users.length; index++) {
            const user = users[index];
            for (let index = 0; index < listingUsers.length; index++) {
                const element = listingUsers[index];
                if (element.value === user) {
                    element.selected = true;
                }
            }
        }
        if (_calls[index].door1) {
            for (let index = 0; index < listingDoors.length; index++) {
                const element = listingDoors[index];
                if (element.value === 'door1')
                    element.selected = true;

            }
        }
        if (_calls[index].door2) {
            for (let index = 0; index < listingDoors.length; index++) {
                const element = listingDoors[index];
                if (element.value === 'door2')
                    element.selected = true;
            }
        }

        listingUsers.addEventListener('change', function () {
            validadeteSaveCallsButton();
        })
        listingDoors.addEventListener('change', function () {
            validadeteSaveCallsButton();
        })
    }
}

function validadeteSaveCallsButton() {
    const btnSaveCall = divCalls.getElementsByClassName('save-button')[0];

    var cards = divCalls.getElementsByClassName('container-card-insert-call');
    for (let index = 0; index < cards.length; index++) {
        const card = cards[index];
        let listingUsers = card.getElementsByTagName('select')[0];
        let validateUsers = false;
        if (listingUsers.selectedOptions.length > 0)
            validateUsers = true;


        let listingDoors = card.getElementsByTagName('select')[1];
        let validateDoors = false;

        if (listingDoors.selectedOptions.length > 0)
            validateDoors = true;


        if (!(validateUsers && validateDoors)) {
            btnSaveCall.disabled = true;
            return true;
        }

    }
    btnSaveCall.disabled = false;

}

async function saveCallsBtn() {
    const btnSaveCall = divCalls.getElementsByClassName('save-button')[0];

    btnSaveCall.addEventListener('click', async function () {
        let calls = [];
        var cards = divCalls.getElementsByClassName('container-card-insert-call');
        for (let index = 0; index < cards.length; index++) {
            const card = cards[index];
            let listingUsers = card.getElementsByTagName('select')[0];
            let numbers = [];
            let door1, door2 = false;
            let listingDoors = card.getElementsByTagName('select')[1];
            for (let index = 0; index < listingUsers.selectedOptions.length; index++) {
                const element = listingUsers.selectedOptions[index];
                numbers.push(element.value);
            }
            for (let index = 0; index < listingDoors.selectedOptions.length; index++) {
                const element = listingDoors.selectedOptions[index];
                if (element.value === "door1") {
                    door1 = true;
                }
                if (element.value === "door2") {
                    door2 = true;
                }
            }
            let call = {};
            call.numbers = numbers;
            call.door1 = door1;
            call.door2 = door2;
            calls.push(call);
        }
        console.log(calls);
        let messageDisplay = divCalls.getElementsByClassName('container-message')[0];
        messageDisplay.style.display = "flex";

        setInterval(function () {
            messageDisplay.style.display = "none";
        }, 2000)
        _configuration.calls = [...calls];
        localStorage.setItem('Configuration', JSON.stringify(_configuration));
        btnSaveCall.disabled = true;
        let myHeaders = new Headers();
        myHeaders = {
            method: 'POST',
            body: JSON.stringify(_configuration)
        };
        await fetch("configuration", myHeaders)
            .then(response => {
                const contentType = response.headers.get('content-type');
                if (!contentType || !contentType.includes('application/json')) {
                    throw new TypeError("Oops, we haven't got JSON!");
                }
                return response.json();
            })
            .then(data => {
                console.log(data);
            })
            .catch(error => console.error(error));
    })

}

function addNewCallBtn() {
    const btnAddCall = divCalls.getElementsByClassName('container-add-new')[0];
    const btnSaveCall = divCalls.getElementsByClassName('save-button')[0];

    btnAddCall.addEventListener('click', function () {
        const divCallsModel = divCalls.getElementsByClassName('container-card-insert-call-model')[0];

        var divNewCall = divCallsModel.cloneNode(true);
        divNewCall.style.display = 'block'
        divNewCall.className = 'container-card-insert-call';
        btnAddCall.parentNode.insertBefore(divNewCall, btnAddCall);

        let btnDeleteCall = divNewCall.getElementsByClassName('icon-trash')[0]
        btnDeleteCall.addEventListener('click', function () {
            this.parentNode.parentNode.parentNode.removeChild(this.parentNode.parentNode);
            btnSaveCall.disabled = false;
        })

        let listingUsers = divNewCall.getElementsByTagName('select')[0];
        let listingDoors = divNewCall.getElementsByTagName('select')[1];

        listingUsers.addEventListener('change', function () {
            validadeteSaveCallsButton();
        })
        listingDoors.addEventListener('change', function () {
            validadeteSaveCallsButton();
        })
    });
}



window.onload = async function () {
    fireBehaviorMenuList();
    _configuration = JSON.parse(localStorage.getItem('Configuration'));
    countersUpdate();
    await getSignalStrength();
    addNewUserBtn();
    saveUserBtn();
    addNewMessageBtn();
    saveMessagesBtn();
    addNewCallBtn();
    saveCallsBtn();
    document.getElementsByTagName('body')[0].style.display = "block";
};