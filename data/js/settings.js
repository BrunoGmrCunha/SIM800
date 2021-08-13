


function fireBehaviorSubMenuList() {
    const subMenuItems = document.getElementsByClassName('setting-item')
    for (let index = 0; index < subMenuItems.length; index++) {
        const element = subMenuItems[index];
        element.addEventListener('click', function () {
            menuRedirect(this.id);
        })

    }
}
function back(){
    if (document.getElementsByClassName('container-settings-menu')[0].style.display=="none"){
        document.getElementsByClassName('container-page-title')[0].style.display = "block";
        document.getElementsByClassName('container-settings-menu')[0].style.display = "block";
        document.getElementsByClassName('container-settings-sim-card')[0].style.display = "none";
        document.getElementsByClassName('container-settings-wifi')[0].style.display = "none";
        document.getElementsByClassName('container-settings-doors-name')[0].style.display = "none";


    }
    else{
        window.location='index.html';
    }
}

function menuRedirect(id) {
    switch (id) {
        case 'sim-card':
            document.getElementsByClassName('container-page-title')[0].style.display = "none";
            document.getElementsByClassName('container-settings-menu')[0].style.display = "none";
            document.getElementsByClassName('container-settings-sim-card')[0].style.display = "block";
            break;
        case 'wifi':
            document.getElementsByClassName('container-page-title')[0].style.display = "none";
            document.getElementsByClassName('container-settings-menu')[0].style.display = "none";
            document.getElementsByClassName('container-settings-wifi')[0].style.display = "block";
            break;
        case 'doors-name':
            document.getElementsByClassName('container-page-title')[0].style.display = "none";
            document.getElementsByClassName('container-settings-menu')[0].style.display = "none";
            document.getElementsByClassName('container-settings-doors-name')[0].style.display = "block";
            break;
        case 'factory-reset':
            confirm("Essa ação apagará todos os seus dados e configurações \n \n Têm a certeza que pretende repor as definições de fábrica?", "Sim", "Não");
            window.location = 'settings.html';
            break;
        default:
            break;
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
    getSignalStrength();
    fireBehaviorSubMenuList();

   
};