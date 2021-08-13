var _configuration, _users;


function addNewCallBtn() {
    const btn = document.getElementsByClassName('container-add-new')[0];
    let saveBtn = document.getElementsByClassName('save-button')[0];

    btn.addEventListener('click', function () {
        const div = document.getElementsByClassName('container-card-insert-call')[0];

        var newDiv = div.cloneNode(true);
        newDiv.style.display = 'block'
        btn.parentNode.insertBefore(newDiv, btn);

        let deleteBtn = newDiv.getElementsByClassName('icon-trash')[0]
        deleteBtn.addEventListener('click', function () {
            this.parentNode.parentNode.parentNode.removeChild(this.parentNode.parentNode);
            saveBtn.disabled = false;
        })

        let listingUsers = newDiv.getElementsByTagName('select')[0];
        let listingDoors = newDiv.getElementsByTagName('select')[1];

        listingUsers.addEventListener('change', function () {
            validadeteSaveButton();
        })
        listingDoors.addEventListener('change', function () {
            validadeteSaveButton();
        })
    });
}





function validadeteSaveButton() {
    let saveBtn = document.getElementsByClassName('save-button')[0];

    var cards = document.getElementsByClassName('container-card-insert-call');
    for (let index = 1; index < cards.length; index++) {
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
            saveBtn.disabled = true;
            return true;
        }

    }
    saveBtn.disabled = false;

}

function saveCallsBtn() {
    let saveBtn = document.getElementsByClassName('save-button')[0];

    saveBtn.addEventListener('click', function () {
        let calls = [];
        var cards = document.getElementsByClassName('container-card-insert-call');
        for (let index = 1; index < cards.length; index++) {
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
        let messageDisplay = document.getElementsByClassName('container-message')[0];
        messageDisplay.style.display = "flex";

        setInterval(function () {
            messageDisplay.style.display = "none";
        }, 2000)
        _configuration.calls = calls;
        localStorage.setItem('Configuration', JSON.stringify(_configuration));
    })

}


function loadCalls() {
    _configuration = JSON.parse(localStorage.getItem('Configuration'));
    const btn = document.getElementsByClassName('container-add-new')[0];
    const saveBtn = document.getElementsByClassName('save-button')[0];

    if (_configuration === null) {
        document.getElementsByClassName('container-message-no-users')[0].style.display = 'flex';
        document.getElementsByClassName('container-save-button')[0].style.display = 'none';
        btn.style.display = 'none';
        return;
    }
    _users = _configuration.users;
    if (_users === undefined || _users.length <= 0) {
        document.getElementsByClassName('container-message-no-users')[0].style.display = 'flex';
        document.getElementsByClassName('container-save-button')[0].style.display = 'none';
        btn.style.display = 'none';
        return;
    }
    const div = document.getElementsByClassName('container-card-insert-call')[0];

    let listingUsers = div.getElementsByTagName('select')[0];
    let listingDoors = div.getElementsByTagName('select')[1];

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
    _calls = _configuration.calls;
    if (_configuration === null)
        return;
    if (_calls === undefined)
        return;


    for (let index = 0; index < _calls.length; index++) {
        const call = _calls[index];
        var newDiv = div.cloneNode(true);
        newDiv.style.display = 'block'
        btn.parentNode.insertBefore(newDiv, btn);

        let deleteBtn = newDiv.getElementsByClassName('icon-trash')[0]
        deleteBtn.addEventListener('click', function () {
            this.parentNode.parentNode.parentNode.removeChild(this.parentNode.parentNode);
            saveBtn.disabled = false;
        })

        let listingUsers = newDiv.getElementsByTagName('select')[0];
        let listingDoors = newDiv.getElementsByTagName('select')[1];
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
            validadeteSaveButton();
        })
        listingDoors.addEventListener('change', function () {
            validadeteSaveButton();
        })
    }
}


function getSignalStrength() {
    signalStrength = localStorage.getItem("SignalStrength");
    const signalStrengthImg = document.getElementsByClassName('icon-signal')[0];
    console.log("signal :", signalStrength);
    if (signalStrength < 2) {
        console.log("0");
        signalStrengthImg.src = 'img/rede_0.svg'
    }
    else if (signalStrength >= 2 && signalStrength < 10) {
        console.log("1");

        signalStrengthImg.src = 'img/rede_1.svg'
    }
    else if (signalStrength>= 10 && signalStrength < 15) {
        console.log("2");

        signalStrengthImg.src = 'img/rede_2.svg'
    }
    else if (signalStrength >= 15 && signalStrength < 20) {
        console.log("3");

        signalStrengthImg.src = 'img/rede_3.svg'
    }
    else if (signalStrength >= 20 && signalStrength <= 30) {
        console.log("4");
        signalStrengthImg.src = 'img/rede_4.svg'
    }
}
window.onload = function () {
    addNewCallBtn();

    loadCalls();
    saveCallsBtn();
    getSignalStrength();
    document.getElementsByTagName('body')[0].style.display = "block";

}