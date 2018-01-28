document.addEventListener("DOMContentLoaded", function (event) {
    fetch("http://localhost:5000/api/unlocked", {
        method: "get"
    }).then(function(res) {
    return res.json();
    }).then(function(json) {
        console.log(json.msg);
        if (json.msg) {
                document.getElementById('autofilldiv').style.display = "block";
                document.getElementById('connected').style.display = "none";
        } else {
            // console.log("ye");
            document.getElementById('autofilldiv').style.display = "none";
            document.getElementById('connected').style.display = "block";
            // window.onload = function () {
            //     console.log("test");
            // }

        }
    })
});


window.onload = function () {
    document.getElementById('connectbtn').onclick = function() {
        console.log(document.getElementById('password').value);
    }
}


document.addEventListener("DOMContentLoaded", function (event) {
    document.getElementById("connectbtn").addEventListener('click', function(){
        fetch("http://localhost:5000/api/password", {
            method: 'post',
            body: JSON.stringify({
                password: document.getElementById('password').value
            })
        });
        // console.log(document.getElementById('password').value);
    });
});