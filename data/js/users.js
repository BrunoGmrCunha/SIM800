var _configuration;
function addNewUserBtn() {
    const btn = document.getElementsByClassName('container-add-new')[0];
    let saveBtn = document.getElementsByClassName('save-button')[0];

    btn.addEventListener('click', function () {
        var div = document.getElementsByClassName('container-card-insert-user')[0];
        var newDiv = div.cloneNode(true);
        newDiv.style.display = 'block'
        btn.parentNode.insertBefore(newDiv, btn);

        let deleteBtn = newDiv.getElementsByClassName('icon-trash')[0]
        deleteBtn.addEventListener('click', function () {
            this.parentNode.parentNode.parentNode.removeChild(this.parentNode.parentNode);
            saveBtn.disabled = false;

        })
        let nameField = newDiv.getElementsByClassName('name-input')[0];
        nameField.addEventListener('change', function () {
            validadeteSaveButton();
        })

        let numberField = newDiv.getElementsByClassName('number-input')[0];

        numberField.addEventListener('change', function () {
            validadeteSaveButton();
        });
    });
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

function validadeteSaveButton() {
    let saveBtn = document.getElementsByClassName('save-button')[0];

    var cards = document.getElementsByClassName('container-card-insert-user');
    for (let index = 1; index < cards.length; index++) {
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

function saveBtn() {
    let saveBtn = document.getElementsByClassName('save-button')[0];

    saveBtn.addEventListener('click', function () {
        let users = [];
        var cards = document.getElementsByClassName('container-card-insert-user');
        for (let index = 1; index < cards.length; index++) {
            const card = cards[index];
            let nameField = card.getElementsByClassName('name-input')[0];
            let numberField = card.getElementsByClassName('number-input')[0];
            let user = {};
            user.name = nameField.value;
            user.number = numberField.value;
            users.push(user);
        }
        console.log(users);
        let message = document.getElementsByClassName('container-message')[0];
        message.style.display = "flex";

        setInterval(function () {
            message.style.display = "none";
        }, 2000)
        _configuration.users = users;
        localStorage.setItem('Configuration', JSON.stringify(_configuration));
        fetch("configuration",
            {
                method: "POST",
                body: JSON.stringify(_configuration)
            })
            .then(function (res) { return res.json(); })
            .then(function (data) { alert(JSON.stringify(data)) });

    })

}


function loadUsers() {
    _configuration = JSON.parse(localStorage.getItem('Configuration'));
    const btn = document.getElementsByClassName('container-add-new')[0];
    let saveBtn = document.getElementsByClassName('save-button')[0];
    if (_configuration === null)
        return;
    let users = _configuration.users;
    if (users === undefined)
        return;

    for (let index = 0; index < users.length; index++) {
        const user = users[index];


        var div = document.getElementsByClassName('container-card-insert-user')[0];
        var newDiv = div.cloneNode(true);
        newDiv.style.display = 'block'
        btn.parentNode.insertBefore(newDiv, btn);

        let deleteBtn = newDiv.getElementsByClassName('icon-trash')[0]
        deleteBtn.addEventListener('click', function () {
            this.parentNode.parentNode.parentNode.removeChild(this.parentNode.parentNode);
            saveBtn.disabled = false;
        })
        let nameField = newDiv.getElementsByClassName('name-input')[0];
        nameField.value = user.name;
        nameField.addEventListener('change', function () {
            validadeteSaveButton();
        })

        let numberField = newDiv.getElementsByClassName('number-input')[0];
        numberField.value = user.number;
        numberField.addEventListener('change', function () {
            validadeteSaveButton();
        });
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
    loadUsers();
    addNewUserBtn();
    saveBtn();
    getSignalStrength();
    document.getElementsByTagName('body')[0].style.display = "block";

};