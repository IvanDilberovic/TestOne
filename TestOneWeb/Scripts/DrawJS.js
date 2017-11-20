window.onload = function () {

    createCanvas();


    $("#btnSave").on('click', function () {

        var result = saveImage();

    })

    $("#btnClear").on('click', function () {

        clearCanvas();

    })

    $("#btnTestRest").on('click', function () {

        var result = TestRest();

    })

}



function createCanvas() {


    var canvasDiv = document.getElementById('canvasDiv');

    canvas = document.createElement('canvas');

    canvas.setAttribute('width', 150);
    canvas.setAttribute('height', 150);
    canvas.style.backgroundColor = 'white';
    canvas.style.border = '2px solid';
    canvas.style.margin = '10px';

    canvas.setAttribute('id', 'canvas');

    canvasDiv.appendChild(canvas);



    if (typeof G_vmlCanvasManager != 'undefined') {
        canvas = G_vmlCanvasManager.initElement(canvas);
    }

    context = canvas.getContext("2d");

    $('#canvas').mousedown(function (e) {

        var mouseX = e.pageX - this.offsetLeft;
        var mouseY = e.pageY - this.offsetTop;

        paint = true;
        addClick(e.pageX - this.offsetLeft, e.pageY - this.offsetTop);
        redraw();

    });

    $('#canvas').mousemove(function (e) {
        if (paint) {
            addClick(e.pageX - this.offsetLeft, e.pageY - this.offsetTop, true);
            redraw();
        }
    });

    $('#canvas').mouseup(function (e) {
        paint = false;
    });

    $('#canvas').mouseleave(function (e) {
        paint = false;
    });



    var clickX = new Array();
    var clickY = new Array();
    var clickDrag = new Array();
    var paint;


    function addClick(x, y, dragging) {
        clickX.push(x);
        clickY.push(y);
        clickDrag.push(dragging);
    }

    function redraw() {
        context.clearRect(0, 0, context.canvas.width, context.canvas.height); // Clears the canvas

        context.strokeStyle = "black";
        context.lineJoin = "round";
        context.lineWidth = 20;

        for (var i = 0; i < clickX.length; i++) {
            context.beginPath();
            if (clickDrag[i] && i) {
                context.moveTo(clickX[i - 1], clickY[i - 1]);
            } else {
                context.moveTo(clickX[i] - 1, clickY[i]);
            }
            context.lineTo(clickX[i], clickY[i]);
            context.closePath();
            context.stroke();
        }
    }


}

function clearCanvas() {

    var canvas = document.getElementById('canvas');
    canvas.outerHTML = "";
    delete canvas;
    createCanvas();

    var canvasDva = document.getElementById('pictureCanvas');
    var context = canvasDva.getContext("2d");

    context.clearRect(0, 0, canvasDva.width, canvasDva.height);

    $("#prvi").empty();
    $("#drugi").empty();
    $("#treci").empty();


}

function saveImage() {

    var canvas = document.getElementById('canvas');

    var slika = canvas.toDataURL();

    //Ovo ak ću slat array cijeli ali to još trebam vidjeti

    //var context = canvas.getContext("2d");

    //var pixels = context.getImageData(0, 0, canvas.width, canvas.height);

    //var i;
    //var s = "";

    //for (i = 0; i < pixels.data.length; i += 4) {

    //    s += pixels.data[i] + ",";
    //    s += pixels.data[i + 1] + ",";
    //    s += pixels.data[i + 2] + ",";
    //    s += pixels.data[i + 3] + ",";

    //}

    //var junk = JSON.stringify(s);

    //$.ajax({
    //    type: "POST",
    //    url: "http://localhost:8080/restdemo",
    //    data: junk,
    //    dataType: 'json',
    //    contentType: 'application/json',
    //    success: function (data) {
    //        handleData(data);
    //        console.log("Success: " + data);
    //        console.log("Probam parse: " + JSON.parse(data));

    //    },
    //    error: function (data) {
    //        console.log("Error: " + data);
    //    }
    //});


    $.ajax({
        type: "POST",
        data: JSON.stringify({ "slika": slika }),
        url: "../Home/SaveImage",
        contentType: 'application/json',
        //dataType: 'html',
        success: function (data) {
            console.log("Success: " + data);
            showImageInCanvas(data);
        },
        error: function (error) {
            console.log("Error: " + error);
        }
    });
}

function showImageInCanvas(data) {

    var canvas = document.getElementById('pictureCanvas');

    var context = canvas.getContext("2d");

    var image = new Image();

    image.onload = function () {
        context.drawImage(image, canvas.width / 2 - image.width / 2, canvas.height / 2 - image.height / 2);
    }

    image.src = data;

    TestRest(data);

}

function TestRest(data) {

    var array = data.split(',');

    var json = JSON.stringify(array[1]);

    console.log("Priprema za POST: " + json);

    $.ajax({
        type: "POST",
        url: "http://localhost:8080/restdemo",
        dataType: 'json',
        contentType: 'application/json',
        success: function (data) {
            handleData(data);
            console.log("Success: " + data);
            console.log("Probam parse: " + JSON.parse(data));

        },
        error: function (data) {
            console.log("Error: " + data);
        }
    });


    //$.ajax({
    //    type: "GET",
    //    url: "http://localhost:8080/restdemo",
    //    data: JSON.stringify(Data),
    //    dataType: 'json',
    //    success: function (data) {
    //        console.log("Success: ");
    //        Object.keys(data).forEach(function (key) {
    //            console.log(data[key]);
    //        });
    //    },
    //    error: function (data) {
    //        console.log("Error: ");
    //        Object.keys(data).forEach(function (key) {
    //            console.log(data[key]);
    //        });
    //    }
    //});

}

function handleData(data) {

    var rezultat = {};

    var arr = [];

    for (var i = 0; i < data.length; i++) {

        rezultat = {
            rezultat: data[i]["rezultat"],
            postotak: data[i]["postotak"]
        }

        arr[i] = rezultat;
    }

    arr.sort();

    $("#prvi").empty();
    $("#prvi").append("Predviđam: " + arr[0].rezultat + " sa postotkom " + arr[0].postotak.toFixed(2) + "%");

    $("#drugi").empty();
    $("#drugi").append("Drugi najbliži: " + arr[1].rezultat + " sa postotkom " + arr[1].postotak.toFixed(2) + "%");

    $("#treci").empty();
    $("#treci").append("Treći najbliži: " + arr[2].rezultat + " sa postotkom " + arr[2].postotak.toFixed(2) + "%");


}