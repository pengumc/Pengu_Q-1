//=====================
// TRACE
//=====================
function trace(text){
    el = document.getElementById("trace");
    if(el.childNodes.length > 40) el.removeChild(el.childNodes[0]);
    el.innerHTML += "<div>"+text+"</div>";
}
//===============
// ERROR CALLBACK
//===============
function simple_error(src, d){
    $('#errors').prepend(
        '<div class="err">'+
        src + ':&nbsp&nbsp&nbsp' +
        d.responseJSON.error+
        '</div>'
    );
}
function simple_success(src, msg){
    $('#errors').prepend(
        '<div class="suc">'+
        src + ':&nbsp&nbsp&nbsp' +
        msg+
        '</div>'
    );
}
function simple_msg(msg){
    $('#errors').prepend(
        '<div class="msg">'+msg+    
        '</div>'
    );
}

/*-----------------------------------TOPBAR ------------------------------------*/
const LIBRARY_FILE = "libcrobot64.dll"
function top_good(selector){
    $(selector).removeClass("topitemred").addClass("topitemgreen");
}

function top_bad(selector){
    $(selector).addClass("topitemred").removeClass("topitemgreen");
}

//======== LIBRARY
function click_library(e){
    if ($(e).hasClass("topitemgreen")){
        //all good nothing to click here
        return
    }else{
        //send load_library
        var req = "load_lib?path="+LIBRARY_FILE
        $.ajax({url:req}).success(function(d){
            simple_success("load_lib","success");
            top_good('#toplib');
        }).error(function(d){
            simple_error(req, d);
            var err= d.responseJSON.error;
            if ( err == "'already loaded'"){
                top_good('#toplib');
            }else{
                top_bad('#toplib');
            }
        });
    }
}

//==========USB
function click_usb(e){
    if ($(e).hasClass("topitemgreen")){
        //all good, nothing to do here
        return
    }else{
        var req = "connect";
        $.ajax({url:req}).success(function(d){
            simple_success("connect", "success");
            top_good('#topusb');
        }).error(function(d){
            simple_error(req, d);
            top_bad("#topusb");
        });
    }
}


/*-------------------------TABS------------------------------------------*/
function click_tab(e){
    //if ($(e).hasClass("taba")) return;
    $('.taba').removeClass("taba").addClass("tab");
    $(e).removeClass("tab").addClass("taba");
    $('.data').hide();
    $('#v_'+ $(e).text()).show();
    if($(e).text() == 'Manual'){
        //redraw servo stuff
    }
}


/*-----------------------------------MANUAL-----------------------------*/
var g_manual_cv = new Array(12);
function setup_manual(){
    var base = $('#v_Manual')
    add_sbox(5, base);
    add_sbox(4, base);
    add_sbox(3, base);
    base.append('<div class="spacer"></div>')
    add_sbox(0, base);
    add_sbox(1, base);
    add_sbox(2, base);
    base.append("<br><br><br>");
    add_sbox(11, base);
    add_sbox(10, base);
    add_sbox(9, base);
    base.append('<div class="spacer"></div>')
    add_sbox(8, base);
    add_sbox(7, base);
    add_sbox(6, base);

    for(var i=0;i<12;i++){
        g_manual_cv[i] = document.getElementById('cvs'+i).getContext("2d");
        setup_canvas(g_manual_cv[i], 50, 50);
        draw_angle(g_manual_cv[i], 0, 5);
    }
}

function update_sboxes(data){
    snoes = $('.sno').each(function(index){
        //$(this) is curren sno now
        //update angle and pw text
        var i = parseInt($(this).text());
        $(this).parent().find('.spw').text(data['pw'][i]);
        var a = data['angles'][i]
        $(this).parent().find('.sangle').text(Math.floor(a*100)/100);
        //redraw servo angle
        draw_angle(g_manual_cv[i], a, 5);
    });
}

function click_sud(e, v){
    if ($('#toplib').hasClass('topitemred')) {
        simple_error("change_servo", {responseJSON:{error:"library state is unkown"}});
        return
    }
    var n = $(e).parent().siblings('.sno').text();
    req = "change_servo?n=" + n + "&v=" + v;
    $.ajax({url:req}).success(function(d){
        simple_success("change_servo " + n + " by " + v, "success");
        update_sboxes(d)
        //redraw servo stuff
    }).error(function(d){
        simple_error(req, d);
    });
}

function add_sbox(n, parent){
    /*other functions assume on this parent/child structure
        .sbox
            .sno
            canvas
            .sdhold
                .spw
                .sangle
            unamed
                .sdown
                .sup
    */
    var el = '<div class="sbox"><div class="sno" >'+n+'</div><canvas id="cvs'+n+'" width="50" height="50"></canvas><div class="sdhold"><div class="spw">72</div><div class="sangle">1.39</div></div><div><div class="sdown" onclick="click_sud(this,-0.1)"><div class="arrow-down"></div></div><div class="sup" onclick="click_sud(this, 0.1)"><div class="arrow-up"></div></div></div></div>'
    parent.append(el);
}

//=====================
// SETUP CANVAS
//=====================
function setup_canvas(ctx, height, width){
    //10px = 1 cm
    /*
        a c e
        b d f
        0 0 1
        I SHIT YOU NOT... THIS IS HOW THEY IMPLEMENTED IT
    */
    ctx.save()
    ctx.transform(10,0,0,-10,width/2,height/2);
    ctx.lineWidth = 0.1;
}


//=====================
// DRAW GRID
//=====================
function draw_grid(ctx, width, height, size, color){
    ctx.beginPath();
    ctx.strokeStyle = color;
    ctx.moveTo(0,0);
    var i;
    for(i=0;i<width;i+=size){
        ctx.moveTo(i, 0);
        ctx.lineTo(i,height);
    }
    for(i=0;i<height;i+=size){
        ctx.moveTo(0, i);
        ctx.lineTo(width,i);
    }
    ctx.stroke();
    ctx.closePath();
}


//=====================
// DRAW LEG
//=====================
function draw_leg(ctx, s1, s2, s3, A,B,C){
    ctx.beginPath();
    ctx.fillStyle="red";
    ctx.fillRect(s1.x-1, s1.y-1, 2,2);
}

//=====================
// DRAW ANGLE
//=====================
function draw_angle(ctx, angle, size){
    ctx.clearRect(-size/2, -size/2, size, size)
    ctx.beginPath();
    ctx.strokeStyle = "black";
    ctx.arc(0,0,size*0.4, 0, Math.PI*2);
    ctx.moveTo(0,0);
    ctx.lineTo(Math.cos(angle)*size*0.4,Math.sin(angle)*size*0.4 );
    ctx.stroke();
}