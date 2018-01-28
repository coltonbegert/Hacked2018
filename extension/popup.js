document.addEventListener("DOMContentLoaded", function (event) {
    fetch("http://localhost:5000/api/unlocked", {
        method: "get"
    }).then(function(res) {
    return res.json();
    }).then(function(json) {
        console.log(json.msg);
        if (json.msg) {
                document.getElementById('autofilldiv').style.display = "inline-block";
                document.getElementById('connected').style.display = "none";
        } else {
            // console.log("ye");
            document.getElementById('autofilldiv').style.display = "none";
            document.getElementById('connected').style.display = "inline-block";
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


document.addEventListener("DOMContentLoaded", function (event) {
    fetch("http://localhost:5000/api/unlocked", {
        method: "get"
    }).then(function (res) {
        return res.json();
    }).then(function (json) {
        console.log(json.msg);
        if (json.msg) {
            document.getElementById('autofilldiv').style.display = "inline-block";
            document.getElementById('connected').style.display = "none";
        } else {
            // console.log("ye");
            document.getElementById('autofilldiv').style.display = "none";
            document.getElementById('connected').style.display = "inline-block";
            // window.onload = function () {
            //     console.log("test");
            // }

        }
    })
});

document.addEventListener("DOMContentLoaded", function (event) {
    fetch("http://localhost:5000/api/toc", {
        method: "get"
    }).then(function (res) {
        return res.json();
    }).then(function (json) {
        console.log(JSON.parse(json))

        var options = {
            valueNames: ['website', 'username', "id"],
            item: '<li class="listitm"><h3 class="website"></h3><p class="username"></p><button class="remove-item-btn">Delete</button><p class="id" style="display:none;">id</p></li>'
        };
        var userList = new List('users', options, JSON.parse(json));


    var removeBtns = $('.remove-item-btn');

    refreshCallbacks();

    function refreshCallbacks() {
        // Needed to add new buttons to jQuery-extended object
        removeBtns = $(removeBtns.selector);

        removeBtns.click(function () {
            // console.log("button");
            var itemId = $(this).closest('li').find('.id').text();
            userList.remove('id', itemId);
        });
    }

    })
    // var values = [];
    //     {
    //         website:"google.com",
    //         username:"begert@ualberta.ca",
    //         id:"1"
    //     }, 
    //     {
    //         website:"reddit",
    //         username:"colton",
    //         id:"2"
    //     }
    // ];

    // var userList = new List('users', options, values);


    // var removeBtns = $('.remove-item-btn');

    // refreshCallbacks();

    // function refreshCallbacks() {
    //     // Needed to add new buttons to jQuery-extended object
    //     removeBtns = $(removeBtns.selector);

    //     removeBtns.click(function () {
    //         // console.log("button");
    //         var itemId = $(this).closest('li').find('.id').text();
    //         userList.remove('id', itemId);
    //     });
    // }

var values = [];
//     {
//         website:"google.com",
//         username:"begert@ualberta.ca",
//         id:"1"
//     }, 
//     {
//         website:"reddit",
//         username:"colton",
//         id:"2"
//     }
// ];

// var userList = new List('users', options, values);


// var removeBtns = $('.remove-item-btn');

//     refreshCallbacks();

// function refreshCallbacks() {
//     // Needed to add new buttons to jQuery-extended object
//     removeBtns = $(removeBtns.selector);

//     removeBtns.click(function () {
//         // console.log("button");
//         var itemId = $(this).closest('li').find('.id').text();
//         userList.remove('id', itemId);
//     });
// }

});

document.addEventListener("DOMContentLoaded", function (event) {
    document.getElementById("manage").addEventListener("click", function(event) {
        console.log("heyo");
        // console.log(document.getElementById("listdiv").style.display);
        // $("listdiv").css('display') === 'none'
        let disp = document.getElementById("listdiv").style.display;
        if (disp != "block") {
            document.getElementById("listdiv").style.display = "block";
        } else {
            document.getElementById("listdiv").style.display = "none"
        }
    })
});

// document.addEventListener("DOMContentLoaded", function (event) {
//     document.getElementById("autofill").addEventListener("click", function (event) {
//         fetch("http://localhost:5000/api/toc", {
//             method: "get"
//         }).then(function (res) {
//             return res.json();
//         }).then(function (json) {
//             var data = JSON.parse(json)
//             chrome.tabs.getSelected(null, function (tab, data) {
//                 var tablink = tab.url;
//                 document.getElementById("add-website").value = tablink;
//                 console.log(tablink, data);
//             });

//             // console.log(JSON.parse(json))

            
//         });
//     })
// });


document.addEventListener("DOMContentLoaded", function (event) {
    document.getElementById("add-btn").addEventListener("click", function (event) {
        console.log("heyo");
        // console.log(document.getElementById("listdiv").style.display);
        // $("listdiv").css('display') === 'none'
        let disp = document.getElementById("adddiv").style.display;
        if (disp != "block") {
            document.getElementById("adddiv").style.display = "block";
        } else {
            document.getElementById("adddiv").style.display = "none"
        }
    })
});

document.addEventListener("DOMContentLoaded", function (event) {
    document.getElementById("add-submit").addEventListener("click", function (event) {
        // console.log("heyo");
        // console.log(document.getElementById("listdiv").style.display);
        // $("listdiv").css('display') === 'none'

        let form = JSON.stringify({
            password:document.getElementById("add-password").value,
            username: document.getElementById("add-username").value,
            website: document.getElementById("add-website").value,
            id: document.getElementById("add-website").value

        });
        console.log(form);
        document.getElementById("add-password").value = "";
        document.getElementById("add-username").value = "";
        document.getElementById("add-website").value = "";

        // userList.add(form);

    })
});

document.addEventListener("DOMContentLoaded", function (event) {
    chrome.tabs.getSelected(null, function (tab) {
        var tablink = tab.url;
        document.getElementById("add-website").value = tablink;
        console.log(tablink);
    });


});