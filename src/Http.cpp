#include <ESP8266WebServer.h>
#include <flash_hal.h>
#include <FS.h>
#include "StreamString.h"
#include <ESP8266HTTPClient.h>
#include <ESP8266httpUpdate.h>
#include "Http.h"
#include "Module.h"
#include "Rtc.h"

ESP8266WebServer *Http::server;
bool Http::isBegin = false;
bool Http::updateAuthFailed = false;

void Http::handleRoot()
{
    if (captivePortal())
    {
        return;
    }
    if (!checkAuth())
    {
        return;
    }

    server->setContentLength(CONTENT_LENGTH_UNKNOWN);
    server->send(200, F("text/html"), "");
    server->sendContent_P(
        PSTR("<!DOCTYPE html><html lang='zh-cn'><head><meta charset='utf-8'/><meta name='viewport'content='width=device-width, initial-scale=1, user-scalable=no'/><title>"));
    server->sendContent(module ? module->getModuleCNName() : F("修复模式"));
    server->sendContent_P(
        PSTR("</title><style type='text/css'>body{font-family:-apple-system,BlinkMacSystemFont,'Microsoft YaHei',sans-serif;font-size:16px;color:#333;line-height:1.75}#body{margin:0 auto;width:80%;max-width:600px}@media screen and (max-width:900px){#body{width:98%}}#nav{text-align:center}#tab>div{display:none}#nav button{background:#eee;border:1px solid #ddd;padding:.7em 1em;cursor:pointer;z-index:1;margin-left:-1px;outline:0}#nav .active{background:#fff}table.gridtable{color:#333;border-width:1px;border-color:#ddd;border-collapse:collapse;margin:auto;margin-top:15px;width:100%}table.gridtable th{border-width:1.5px;padding:8px;border-style:solid;border-color:#ddd;background-color:#f5f5f5}table.gridtable td{border-width:1px;padding:8px;border-style:solid;border-color:#ddd;background-color:#fff}input,select{border:1px solid #ccc;padding:7px 0;border-radius:3px;padding-left:5px;-webkit-box-shadow:inset 0 1px 1px rgba(0,0,0,.075);box-shadow:inset 0 1px 1px rgba(0,0,0,.075);-webkit-transition:border-color ease-in-out .15s,-webkit-box-shadow ease-in-out .15s;-o-transition:border-color ease-in-out .15s,box-shadow ease-in-out .15s;transition:border-color ease-in-out .15s,box-shadow ease-in-out .15s}input:focus,select:focus{border-color:#66afe9;outline:0;-webkit-box-shadow:inset 0 1px 1px rgba(0,0,0,.075),0 0 8px rgba(102,175,233,.6);box-shadow:inset 0 1px 1px rgba(0,0,0,.075),0 0 8px rgba(102,175,233,.6)}#tab button{color:#fff;border-width:0;border-radius:3px;cursor:pointer;outline:0;font-size:17px;line-height:2.4rem;width:100%}#tab button[disabled]{cursor:not-allowed;filter:alpha(opacity=65);-webkit-box-shadow:none;box-shadow:none;opacity:.65}.btn-info{background-color:#5bc0de;border-color:#46b8da}.btn-info:hover{background-color:#31b0d5;border-color:#269abc}.btn-success{background-color:#5cb85c;border-color:#4cae4c}.btn-success:hover{background-color:#449d44;border-color:#398439}.btn-danger{background-color:#d9534f;border-color:#d43f3a}.btn-danger:hover{background-color:#c9302c;border-color:#ac2925}.alert{width:80%;padding:15px;border:1px solid transparent;border-radius:4px;position:fixed;top:10px;left:10%;z-index:999999;display:none}label.bui-radios-label input{position:absolute;opacity:0;visibility:hidden}label.bui-radios-label .bui-radios{display:inline-block;position:relative;width:13px;height:13px;background:#fff;border:1px solid #979797;border-radius:50%;vertical-align:-2px}label.bui-radios-label input:checked+.bui-radios:after{position:absolute;content:'';width:7px;height:7px;background-color:#fff;border-radius:50%;top:3px;left:3px}label.bui-radios-label input:checked+.bui-radios{background:#00b066;border:1px solid #00b066}label.bui-radios-label input:disabled+.bui-radios{background-color:#e8e8e8;border:solid 1px #979797}label.bui-radios-label input:disabled:checked+.bui-radios:after{background-color:#c1c1c1}label.bui-radios-label .bui-radios{-webkit-transition:background-color ease-out .3s;transition:background-color ease-out .3s}input[type='range']{width:80%;height:10px;border:0;background-color:#f0f0f0;border-radius:5px;position:relative;-webkit-appearance:none!important;outline:0}input[type=range]::-webkit-slider-thumb{-webkit-appearance:none;width:20px;height:20px;border-radius:50%;background:#f40}.file{position:relative;display:inline-block;background:#d0eeff;border:1px solid #99d3f5;border-radius:4px;padding:4px 12px;overflow:hidden;color:#1e88c7;text-decoration:none;text-indent:0;line-height:20px}.file input{position:absolute;font-size:100px;right:0;top:0;opacity:0}.file:hover{background:#aadffd;border-color:#78c3f3;color:#004974;text-decoration:none}</style>"
             "<script type='text/javascript'>"
             "var timer;var logIndex=0;var defIntervalTime=3000;var intervalTime=defIntervalTime;var lt;function id(d){return document.getElementById(d)}function tab(v){var divs=id('tab').childNodes;var btns=id('nav').getElementsByTagName('button');for(var i=0;i<divs.length;i++){divs[i].style.display=divs[i]==id('tab'+v)?'block':'none';btns[i].setAttribute('class',(i+1==v?'active':''))}intervalTime=v==5?1000:defIntervalTime}function serialize(form){var field,s='';if(typeof form=='object'&&form.nodeName=='FORM'){for(var i=0;i<form.elements.length;i++){field=form.elements[i];if(field.name&&!field.disabled&&field.type!='file'&&field.type!='reset'&&field.type!='submit'&&field.type!='button'){if((field.type!='checkbox'&&field.type!='radio')||field.checked){s+=field.name+'='+encodeURIComponent(field.value)+'&'}}}}if(s.length>1){s=s.substring(0,s.length-1)}return s}function ajax(){var ajaxData={type:(arguments[0].type||'GET').toUpperCase(),url:arguments[0].url||'',data:arguments[0].data||null,success:arguments[0].success||function(){},error:arguments[0].error||function(){}};var xhr=new XMLHttpRequest();xhr.responseType='json';xhr.open(ajaxData.type,ajaxData.url);if(ajaxData.type=='POST'){xhr.setRequestHeader('Content-Type','application/x-www-form-urlencoded; charset=utf-8');xhr.send(ajaxData.data)}else{xhr.send()}xhr.onreadystatechange=function(){if(xhr.readyState==4){if(xhr.status==200){ajaxData.success(xhr.response)}else{ajaxData.error()}if(ajaxData.url=='/get_status'){lt=setTimeout(get_status,intervalTime)}}}}function toast(msg,duration,isok){var m=id('alert');m.innerHTML=msg;m.style.cssText=isok?'color: #3c763d;background-color: #dff0d8;border-color: #d6e9c6;':'color: #a94442; background-color: #f2dede; border-color: #ebccd1;';m.style.display='block';clearTimeout(timer);timer=setTimeout(function(){var d=0.5;m.style.webkitTransition='-webkit-transform '+d+'s ease-in, opacity '+d+'s ease-in';m.style.opacity='0';setTimeout(function(){m.style.display='none'},d*1000)},duration)}function postupdate(the){var form=new FormData();form.append('file',document.getElementsByName('update')[0].files[0]);var xhr=new XMLHttpRequest();xhr.responseType='json';xhr.open('POST',the.getAttribute('action'),true);xhr.onreadystatechange=function(){if(xhr.readyState==4){if(xhr.status==200){if(xhr.response.msg){toast(xhr.response.msg,xhr.response.code?5000:9000,xhr.response.code)}if(xhr.response.code){setTimeout('location.reload();',7000)}}else{toast('<strong>Oh snap!</strong> 请求出错！',9000,false)}}};xhr.upload.onprogress=function(ev){if(ev.lengthComputable){toast('已上传'+Math.floor(100*ev.loaded/ev.total)+'%，请耐心等待，稍后将弹出升级情况。',16000,1)}};xhr.send(form)}function postform(the){ajaxPost(the.getAttribute('action'),serialize(the));return false}function getRadioValue(radioName){var radios=document.getElementsByName(radioName);for(var i=0;i<radios.length;i++){var radio=radios.item(i);if(radio.checked){return radio.value}}return undefined}function setRadioValue(radioName,value){var radios=document.getElementsByName(radioName);for(var i=0;i<radios.length;i++){var radio=radios.item(i);if(radio.value==value){radio.checked=true;return}}}function ajaxPost(url,data,callback){ajax({type:'POST',url:url,dataType:'json',data:data,success:function(data){if(typeof(callback)=='function'){if(callback(data)===true){return}}if(data.msg){toast(data.msg,data.code?5000:9000,data.code)}if(data.data){setData(data.data)}},error:function(){toast('<strong>Oh snap!</strong> 请求出错！',9000,false)}})}function get_status(){clearTimeout(lt);ajaxPost('/get_status','i='+logIndex)}window.addEventListener('load',get_status);"
             "function setData(data){for(var key in data){if(typeof(setDataSub)=='function'){var result=setDataSub(data,key);if(result){continue}}var v=data[key];if(key=='logindex'){logIndex=v}else if(key=='log'){if(v){id('log').value+=v;id('log').scrollTop=99999}}else if(key=='ip'){if(v&&v!=window.location.hostname){toast('连接WiFi成功，IP地址：'+v,5000,1);window.setTimeout('location.href=\\'http://'+v+'\\'',5000)}}else{if(id(key)){id(key).innerHTML=v}else{console.log(key)}}}}"
             "</script></head><body><div id='body'><div id='alert' class='alert'></div><h1 style='text-align:center'>"));
    server->sendContent(module ? module->getModuleCNName() : F("修复模式"));
    server->sendContent_P(
        PSTR("</h1><div id='nav'>"
             "<button onclick='tab(1)'class='active'>状态</button>"
             "<button onclick='tab(2)'>联网</button>"
             "<button onclick='tab(3)'>控制</button>"
             "<button onclick='tab(4)'>关于</button>"
#ifdef WEB_LOG_SIZE
             "<button onclick='tab(5)'>日志</button>"
#endif
             "</div>"));

    // TAB 1 Start
    uint8_t mode = WiFi.getMode();
    snprintf_P(tmpData, sizeof(tmpData),
               PSTR("<div id='tab'>"
                    "<div id='tab1' style='display: block;'>"
                    "<table class='gridtable'><thead><tr><th colspan='2'>WiFi状态</th></tr></thead><tbody>"
                    "<tr><td>主机名</td><td>%s</td></tr>"
                    "<tr><td>WiFi模式</td><td>%s</td></tr>"
                    "<tr><td>SSID</td><td>%s</td></tr>"),
               UID,
               (mode == WIFI_STA ? PSTR("STA") : (mode == WIFI_AP ? PSTR("AP") : PSTR("AP STA"))),
               WiFi.SSID().c_str());
    server->sendContent_P(tmpData);

    snprintf_P(tmpData, sizeof(tmpData),
               PSTR("<tr><td>RSSI</td><td>%ddBm</td></tr>"
                    "<tr><td>开机时间</td><td id='uptime'>%s</td></tr>"
                    "<tr><td>空闲内存</td><td><span id='free_mem'>%d</span> kB</td></tr>"
                    "<tr><td>IP地址</td><td>%s</td></tr>"
                    "<tr><td>DHCP</td><td>%s</td></tr>"
                    "</tbody></table>"
                    "</div>"),
               WiFi.RSSI(), Rtc::msToHumanString(millis()).c_str(), ESP.getFreeHeap() / 1024,
               WiFi.localIP().toString().c_str(), (globalConfig.wifi.is_static ? PSTR("静态IP") : PSTR("DHCP")));
    server->sendContent_P(tmpData);
    // TAB 1 End

    // TAB 2 Start
    server->sendContent_P(
        PSTR("<div id='tab2'>"
             "<form method='post' action='/wifi' onsubmit='postform(this);return false'>"
             "<table class='gridtable'><thead><tr><th>WiFi名称</th><th>信号</th></tr></thead><tbody>"
             "<tr id='clusss'><td>WiFi名称</td><td><input type='text' id='wifi_ssid' name='wifi_ssid'></td></tr>"
             "<tr><td>WiFi密码</td><td><input type='text' name='wifi_password'></td></tr>"
             "<tr><td colspan='2'><button type='submit' class='btn-info'>连接WiFi</button></td></tr>"
             "<tr><td colspan='2'><button type='button' class='btn-danger' onclick='scanWifi()'>搜索WiFi</button></td></tr>"
             "</tbody></table></form>"
             "<script type='text/javascript'>function clickwifi(t){id('wifi_ssid').value=t.value}function scanWifi(){ajaxPost('scan_wifi','',function(data){if(data.code==1){if(data.data.list.length==0){scanWifi();return;}var trs=document.getElementsByClassName('addwifi');for(var i=trs.length-1;i>=0;i--){trs[i].remove()}for(var a in data.data.list){var w=data.data.list[a];var tr=document.createElement(\"tr\");var td=document.createElement(\"td\");tr.setAttribute('class','addwifi');td.innerHTML=\"<label class='bui-radios-label'><input type='radio' name='wifi' onclick='clickwifi(this)' value='\"+w.name+\"'/><i class='bui-radios'></i> \"+w.name+(w.type==7?' [开放]':'')+\"</label>\";tr.appendChild(td);td=document.createElement(\"td\");td.innerHTML=w.rssi+'dBm '+w.quality+'%';tr.appendChild(td);var oldEle=id('clusss');oldEle.parentNode.insertBefore(tr,oldEle)}}else{toast(data.msg,data.code?5000:9000,data.code)}})}</script>"));

    server->sendContent_P(
        PSTR("<form method='post' action='/dhcp' onsubmit='postform(this);return false'>"
             "<table class='gridtable'><thead><tr><th colspan='2'>WIFI高级设置</th></tr></thead><tbody>"
             "<tr><td>DHCP</td><td>"
             "<label class='bui-radios-label'><input type='radio' name='dhcp' value='1' onchange='dhcponchange(this)'/><i class='bui-radios'></i> DHCP</label>&nbsp;&nbsp;&nbsp;&nbsp;"
             "<label class='bui-radios-label'><input type='radio' name='dhcp' value='2' onchange='dhcponchange(this)'/><i class='bui-radios'></i> 静态IP</label>"
             "</td></tr>"));

    snprintf_P(tmpData, sizeof(tmpData),
               PSTR("<tr class='dhcp_hide'><td>静态IP</td><td><input type='text' name='static_ip' value='%s'></td></tr>"
                    "<tr class='dhcp_hide'><td>子网掩码</td><td><input type='text' name='static_netmask' value='%s'></td></tr>"
                    "<tr class='dhcp_hide'><td>网关</td><td><input type='text' name='static_gateway' value='%s'></td></tr>"),
               globalConfig.wifi.ip, globalConfig.wifi.sn, globalConfig.wifi.gw);
    server->sendContent_P(tmpData);

    server->sendContent_P(
        PSTR("<tr><td colspan='2'><button type='submit' class='btn-info'>保存</button></td></tr>"
             "</tbody></table></form>"
             "<script type='text/javascript'>function dhcponchange(the){var v=getRadioValue('dhcp');var dom=document.getElementsByClassName('dhcp_hide');for(var i=0;i<dom.length;i++){dom[i].style.display=v==2?'':'none'}}</script>"));

    server->sendContent_P(
        PSTR("<form method='post' action='/http' onsubmit='return checkHttpForm(this)' novalidate>"
             "<table class='gridtable'><thead><tr><th colspan='2'>WEB安全设置</th></tr></thead><tbody>"));
    snprintf_P(tmpData, sizeof(tmpData),
               PSTR("<tr><td>端口</td><td><input type='number' min='1' max='65535' name='http_port' required value='%d' style='width:70px'></td></tr>"
                    "<tr><td>用户名</td><td><input type='text' name='http_user' minlength='2' maxlength='6' value='%s' style='width:180px'></td></tr>"
                    "<tr><td>密码</td><td><input type='password' name='http_pass' minlength='8' maxlength='24' placeholder='留空则不修改' style='width:180px'></td></tr>"),
               globalConfig.http.port, globalConfig.http.user);
    server->sendContent_P(tmpData);
    // 注意: 以上内容超过 tmpData 的一半后必须拆分, 避免 snprintf 截断产生残缺 HTML 标签,
    // 导致后续 tab(控制/关于/日志) 被嵌套进 tab2 并在联网页同时显示
    server->sendContent_P(
        PSTR("<tr><td colspan='2'><button type='submit' class='btn-info'>保存</button></td></tr>"
             "</tbody></table></form>"
             "<script type='text/javascript'>var httpAuthEnabled="));
    // novalidate 已禁用浏览器原生约束气泡, 长度校验改由 checkHttpForm 用页面 toast 提示;
    // 用户名和密码同时留空时关闭登录验证 (需二次确认)
    server->sendContent(String(globalConfig.http.user[0] != 0 && globalConfig.http.pass[0] != 0 ? 1 : 0));
    // 长度校验按 UTF-8 字节数, 与后端 Http::handleHttp 的校验口径一致,
    // 避免中文用户名/密码在前端按字符数通过、后端按字节数拒绝的不一致
    server->sendContent_P(
        PSTR(";function blen(s){var b=0;for(var i=0;i<s.length;i++){var c=s.charCodeAt(i);"
             "if(c<0x80){b+=1}else if(c<0x800){b+=2}else if(c>=0xD800&&c<=0xDFFF){b+=4;i++}else{b+=3}}return b}"));
    server->sendContent_P(
        PSTR("function checkHttpForm(the){var u=the.http_user.value,p=the.http_pass.value;"
             "if(!u&&!p){if(httpAuthEnabled){if(!confirm('确定要关闭登录验证吗？')){return false}}postform(the);return false}"
             "if(u&&(blen(u)<2||blen(u)>6)){toast('用户名长度须为 2~6 位',9000,false);return false}"
             "if(p&&(blen(p)<8||blen(p)>24)){toast('密码长度须为 8~24 位',9000,false);return false}"
             "if(u&&!/^[A-Za-z0-9_\\-@.#]+$/.test(u)){toast('用户名仅限字母数字及 _ - . @ #',9000,false);return false}"
             "if(p&&!/^[A-Za-z0-9_\\-@!#$%\\^*.+?=:~ |]+$/.test(p)){toast('密码仅限字母数字、空格、| 及 _ - . @ ! # $ % ^ * + = ? : ~',9000,false);return false}"
             "postform(the);return false}"
             "</script>"));

    server->sendContent_P(PSTR("</div>"));
    // TAB 2 End

    // TAB 3 Start
    server->sendContent_P(PSTR("<div id='tab3'>"));

    if (module)
    {
        module->httpHtml(server);
    }

    server->sendContent_P(
        PSTR("<form method='post' action='/module_setting' onsubmit='postform(this);return false'>"
             "<table class='gridtable'><thead><tr><th colspan='2'>模块设置</th></tr></thead><tbody>"));

    snprintf_P(tmpData, sizeof(tmpData), PSTR("<tr><td>主机名</td><td><input type='text' name='uid' value='%s'>&nbsp;具有唯一性，留空默认</td></tr>"), UID);
    server->sendContent_P(tmpData);

    server->sendContent_P(
        PSTR("<tr><td>日志输出</td><td>"
             "<label class='bui-radios-label'><input type='checkbox' name='log_serial' value='1'/><i class='bui-radios' style='border-radius:20%'></i> Serial</label>&nbsp;&nbsp;&nbsp;&nbsp;"
             "<label class='bui-radios-label'><input type='checkbox' name='log_serial1' value='1'/><i class='bui-radios' style='border-radius:20%'></i> Serial1</label>&nbsp;&nbsp;&nbsp;&nbsp;"
#ifdef USE_SYSLOG
             "<label class='bui-radios-label'><input type='checkbox' name='log_syslog' value='1'/><i class='bui-radios' style='border-radius:20%'></i> syslog</label>&nbsp;&nbsp;&nbsp;&nbsp;"
#endif
#ifdef WEB_LOG_SIZE
             "<label class='bui-radios-label'><input type='checkbox' name='log_web' value='1'/><i class='bui-radios' style='border-radius:20%'></i> web</label>&nbsp;&nbsp;&nbsp;&nbsp;"
#endif
             "</td></tr>"));

#ifdef USE_SYSLOG
    snprintf_P(tmpData, sizeof(tmpData),
               PSTR("<tr><td>syslog服务器</td><td>"
                    "<input type='text' name='log_syslog_host' style='width:150px' value='%s'> : "
                    "<input type='number' name='log_syslog_port' value='%d' min='0' max='65000' style='width:50px'>"
                    "</td></tr>"),
               globalConfig.debug.server, globalConfig.debug.port);
    server->sendContent_P(tmpData);
#endif

    snprintf_P(tmpData, sizeof(tmpData),
               PSTR("<tr><td>NTP服务器</td><td>"
                    "<input type='text' name='ntp' style='width:150px' value='%s'> 建议在获取时间失败时才填写"
                    "</td></tr>"),
               globalConfig.wifi.ntp);
    server->sendContent_P(tmpData);

    server->sendContent_P(
        PSTR("<tr><td colspan='2'><button type='submit' class='btn-info'>设置</button></td></tr>"
             "</tbody></table></form>"
             "<div>"
             "<button type='button' class='btn-danger' style='margin-top: 10px' onclick=\"javascript:if(confirm('确定要重启模块？')){ajaxPost('/operate', 'd=1');}\">重启模块</button>"
             "<button type='button' class='btn-danger' style='margin-top: 10px' onclick=\"javascript:if(confirm('确定要重置模块？')){ajaxPost('/operate', 'd=2');}\">重置模块</button>"
             "</div>"
             "</div>"));
    // TAB 3 End

    // TAB 4 Start
    server->sendContent_P(
        PSTR("<div id='tab4'>"
             "<table class='gridtable'><thead><tr><th colspan='2'>硬件参数</th></tr></thead><tbody>"));

    snprintf_P(tmpData, sizeof(tmpData),
               PSTR("<tr><td>ESP芯片ID</td><td>%d</td></tr>"
                    "<tr><td>Flash芯片 ID</td><td>%d</td></tr>"
                    "<tr><td>Flash大小</td><td>%d kB</td></tr>"
                    "<tr><td>固件Flash大小</td><td>%d kB</td></tr>"
                    "<tr><td>固件大小</td><td>%d kB</td></tr>"),
               ESP.getChipId(), ESP.getFlashChipId(), ESP.getFlashChipRealSize() / 1024, ESP.getFlashChipSize() / 1024, ESP.getSketchSize() / 1024);
    server->sendContent_P(tmpData);

    uint8_t mac[6];
    wifi_get_macaddr(STATION_IF, mac);
    snprintf_P(tmpData, sizeof(tmpData),
               PSTR("<tr><td>空闲程序空间</td><td>%d kB</td></tr>"
                    "<tr><td>内核和SDK版本</td><td>" ARDUINO_ESP8266_RELEASE "%s</td></tr>"
                    "<tr><td>重启原因</td><td>%s</td></tr>"
                    "<tr><td>MAC地址</td><td>%02X:%02X:%02X:%02X:%02X:%02X</td></tr>"
                    "</tbody></table>"),
               ESP.getFreeSketchSpace() / 1024, ESP.getSdkVersion(), ESP.getResetReason().c_str(), mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    server->sendContent_P(tmpData);

    snprintf_P(tmpData, sizeof(tmpData),
               PSTR("<table class='gridtable'><thead><tr><th colspan='2'>固件升级</th></tr></thead><tbody>"
                    "<tr><td>当前版本</td><td>v%s</td></tr>"
                    "<tr><td>编译时间</td><td>%s</td></tr>"),
               module ? module->getModuleVersion().c_str() : PSTR("0"), Rtc::GetBuildDateAndTime().c_str());
    server->sendContent_P(tmpData);

    server->sendContent_P(
        PSTR("<form method='POST' action='/update' enctype='multipart/form-data' onsubmit='postupdate(this);return false'>"
             "<tr><td colspan='2'><a class='file'><input type='file' name='update'>选择文件</a></td></tr>"
             "<tr><td colspan='2'><button type='submit' class='btn-info'>升级</button><br>"
             "</form>"
             "<tr><td colspan='2' style='text-align:center'>OTA更新</td></tr>"
             "<form method='POST' action='/ota' onsubmit='postform(this);return false'>"
             "<tr><td>OTA地址</td><td><input type='text' name='ota_url' value='" OTA_URL "' style='width:98%'></td></tr>"
             "<tr><td colspan='2'><button type='submit' class='btn-success' onclick=\"return confirm('确定要OTA更新？')\">OTA更新</button></td></tr>"
             "</form>"
             "</tbody></table>"
             "</div>"));
    // TAB 4 End

#ifdef WEB_LOG_SIZE
    // TAB 5 Start
    server->sendContent_P(
        PSTR("<div id='tab5'>"
             "<div style='display:inline-block;color:#000000;min-width:340px;position:absolute;left:1%;margin-top:20px;width:99%'>"
             "<textarea readonly id='log' cols='340' wrap='off' style='resize:none;width:98%;height:600px;padding:5px;overflow:auto;background:#ffffff;color:#000000;'></textarea>"
             "</div></div>"));
// TAB 5 End
#endif

    server->sendContent_P(
        PSTR("</div><div style='text-align:center;margin-top:20px'>开发者：<a href='https://github.com/SheYu09' target='_blank'>舍予 • 小波</a></div><div></body></html>"));

    // TAB 2
    snprintf_P(tmpData, sizeof(tmpData), PSTR("<script type='text/javascript'>%ssetRadioValue('dhcp', '%d');dhcponchange(null);"),
               WiFi.isConnected() ? PSTR("") : PSTR("scanWifi();"), globalConfig.wifi.is_static ? 2 : 1);
    server->sendContent_P(tmpData);

    // TAB 3
    if ((1 & globalConfig.debug.type) == 1)
    {
        server->sendContent_P(PSTR("setRadioValue('log_serial', '1');"));
    }
#ifdef USE_SYSLOG
    if ((2 & globalConfig.debug.type) == 2)
    {
        server->sendContent_P(PSTR("setRadioValue('log_syslog', '1');"));
    }
#endif
#ifdef WEB_LOG_SIZE
    if ((4 & globalConfig.debug.type) == 4)
    {
        server->sendContent_P(PSTR("setRadioValue('log_web', '1');"));
    }
#endif
    if ((8 & globalConfig.debug.type) == 8)
    {
        server->sendContent_P(PSTR("setRadioValue('log_serial1', '1');"));
    }
    server->sendContent_P(PSTR("</script>"));
}

void Http::handledhcp()
{
    if (!checkAuth())
    {
        return;
    }
    String ip = server->arg(F("static_ip"));
    String netmask = server->arg(F("static_netmask"));
    String gateway = server->arg(F("static_gateway"));
    if (!Wifi::isIp(ip))
    {
        server->send_P(200, PSTR("text/html"), PSTR("{\"code\":0,\"msg\":\"IP地址错误\"}"));
        return;
    }
    if (!Wifi::isIp(netmask))
    {
        server->send_P(200, PSTR("text/html"), PSTR("{\"code\":0,\"msg\":\"掩码地址错误\"}"));
        return;
    }
    if (!Wifi::isIp(gateway))
    {
        server->send_P(200, PSTR("text/html"), PSTR("{\"code\":0,\"msg\":\"网关地址错误\"}"));
        return;
    }

    IPAddress static_ip;
    IPAddress static_sn;
    IPAddress static_gw;
    static_ip.fromString(ip);
    static_sn.fromString(netmask);
    static_gw.fromString(gateway);

    if (!(static_ip.isV4() && static_sn.isV4() && (!static_gw.isSet() || static_gw.isV4())))
    {
        server->send_P(200, PSTR("text/html"), PSTR("{\"code\":0,\"msg\":\"IP地址或者网关错误\"}"));
        return;
    }

    if ((static_ip.v4() & static_sn.v4()) != (static_gw.v4() & static_sn.v4()))
    {
        server->send_P(200, PSTR("text/html"), PSTR("{\"code\":0,\"msg\":\"网段错误\"}"));
        return;
    }

    bool old = globalConfig.wifi.is_static;
    globalConfig.wifi.is_static = server->arg(F("dhcp")).equals(F("2"));
    strcpy(globalConfig.wifi.ip, ip.c_str());
    strcpy(globalConfig.wifi.sn, netmask.c_str());
    strcpy(globalConfig.wifi.gw, gateway.c_str());
    Config::saveConfig();

    if (old != globalConfig.wifi.is_static)
    {
        server->send_P(200, PSTR("text/html"), PSTR("{\"code\":1,\"msg\":\"设置DHCP信息成功，重启后生效\"}"));
    }
    else
    {
        server->send_P(200, PSTR("text/html"), PSTR("{\"code\":1,\"msg\":\"设置DHCP信息成功\"}"));
    }
}

// 用户名登录凭证字符集白名单: 仅允许字母数字及 _ - . @ #, 排除 ' " < > & 等
// HTML/JS 特殊字符, 防止值回显到页面时产生 HTML 注入/XSS
static bool isSafeUserNameChar(char c)
{
    if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9'))
    {
        return true;
    }
    switch (c)
    {
    case '_':
    case '-':
    case '.':
    case '@':
    case '#':
        return true;
    default:
        return false;
    }
}

// WEB 登录凭证字符集白名单: 仅允许字母数字及安全符号, 排除 ' " < > & 等
// HTML/JS 特殊字符, 防止值回显到页面时产生 HTML 注入/XSS
static bool isSafeCredChar(char c)
{
    if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9'))
    {
        return true;
    }
    switch (c)
    {
    case '_':
    case '-':
    case '.':
    case '@':
    case '!':
    case '#':
    case '$':
    case '%':
    case '^':
    case '*':
    case '+':
    case '=':
    case '?':
    case ':':
    case '~':
    case ' ':
    case '|':
        return true;
    default:
        return false;
    }
}

void Http::handleHttp()
{
    if (!checkAuth())
    {
        return;
    }
    // 端口 0 或越界时兜底为 80, 避免 server->begin(0) 端口异常
    // 先以 int 接收, 避免 uint16_t 截断导致负数/超大值被静默接受
    int new_port = server->arg(F("http_port")).toInt();
    if (new_port < 1 || new_port > 65535)
    {
        new_port = 80;
    }
    bool port_changed = ((uint16_t)new_port != globalConfig.http.port);
    // 端口值稍后在校验全部通过后再写入 globalConfig, 避免校验失败返回时
    // 内存中的端口已被修改, 又被 60 秒定时保存意外持久化

    // 用户名填写时须 2~6 位; 密码填写时须 8~24 位; 两者留空表示关闭登录验证
    String user = server->arg(F("http_user"));
    if (user.length() > 0 && (user.length() < 2 || user.length() > 6))
    {
        server->send_P(200, PSTR("text/html"), PSTR("{\"code\":0,\"msg\":\"用户名长度须为 2~6 位\"}"));
        return;
    }
    // 密码填写时须 8~24 位; 留空表示不修改 (pass 数组容量 25 字符)
    String pass = server->arg(F("http_pass"));
    if (pass.length() > 0 && (pass.length() < 8 || pass.length() > 24))
    {
        server->send_P(200, PSTR("text/html"), PSTR("{\"code\":0,\"msg\":\"密码长度须为 8~24 位\"}"));
        return;
    }
    // 填写内容仅允许白名单字符, 排除 ' " < > & 等 HTML/JS 特殊字符,
    // 防止值回显到页面时产生 HTML 注入/XSS; 留空字段不校验
    for (size_t i = 0; i < user.length(); i++)
    {
        if (!isSafeUserNameChar(user[i]))
        {
            server->send_P(200, PSTR("text/html"), PSTR("{\"code\":0,\"msg\":\"用户名含非法字符, 仅限字母数字及 _ - . @ #\"}"));
            return;
        }
    }
    for (size_t i = 0; i < pass.length(); i++)
    {
        if (!isSafeCredChar(pass[i]))
        {
            server->send_P(200, PSTR("text/html"), PSTR("{\"code\":0,\"msg\":\"密码含非法字符, 仅限字母数字、空格、| 及 _ - . @ ! # $ % ^ * + = ? : ~\"}"));
            return;
        }
    }
    // 用户名和密码同时留空 → 关闭登录验证 (忘记密码时可在 AP 配网界面免登录操作)
    bool auth_was_enabled = (globalConfig.http.user[0] != 0 && globalConfig.http.pass[0] != 0);
    if (user.length() == 0 && pass.length() == 0)
    {
        if (auth_was_enabled)
        {
            globalConfig.http.user[0] = 0;
            globalConfig.http.pass[0] = 0;
            Debug::AddInfo(PSTR("HTTP auth disabled"));
        }
        globalConfig.http.port = (uint16_t)new_port;
        Config::saveConfig();
        server->send_P(200, PSTR("text/html"),
                       auth_was_enabled ? PSTR("{\"code\":1,\"msg\":\"登录验证已关闭\"}") : PSTR("{\"code\":1,\"msg\":\"已经修改成功\"}"));
        return;
    }
    // 认证启用要求用户名与密码同时非空: 首次设置时只填其一, 认证不会生效,
    // 这里直接拒绝, 避免用户"以为设了密码保护, 实际无任何保护"的误配置
    if (user.length() > 0 && pass.length() == 0 && globalConfig.http.pass[0] == 0)
    {
        server->send_P(200, PSTR("text/html"), PSTR("{\"code\":0,\"msg\":\"首次启用认证须同时填写用户名和密码\"}"));
        return;
    }
    if (pass.length() > 0 && user.length() == 0 && globalConfig.http.user[0] == 0)
    {
        server->send_P(200, PSTR("text/html"), PSTR("{\"code\":0,\"msg\":\"首次启用认证须同时填写用户名和密码\"}"));
        return;
    }
    if (user.length() > 0)
    {
        strcpy(globalConfig.http.user, user.c_str());
    }
    if (pass.length() > 0)
    {
        strcpy(globalConfig.http.pass, pass.c_str());
    }
    globalConfig.http.port = (uint16_t)new_port;
    Config::saveConfig();
    if (port_changed)
    {
        server->send_P(200, PSTR("text/html"), PSTR("{\"code\":1,\"msg\":\"设置WEB授权成功，端口修改重启后生效\"}"));
    }
    else
    {
        server->send_P(200, PSTR("text/html"), PSTR("{\"code\":1,\"msg\":\"设置WEB授权成功\"}"));
    }
}

void Http::handleScanWifi()
{
    if (!checkAuth())
    {
        return;
    }
    int n = WiFi.scanNetworks();
    if (n == 0)
    {
        server->send_P(200, PSTR("text/html"), PSTR("{\"code\":1,\"msg\":\"\",\"data\":{\"list\":[]}}"));
        //server->send_P(200, PSTR("text/html"), PSTR("{\"code\":0,\"msg\":\"找不到网络，请重新试试。\"}"));
        return;
    }

    //sort networks
    int indices[n];
    for (int i = 0; i < n; i++)
    {
        indices[i] = i;
    }

    // RSSI排序
    for (int i = 0; i < n; i++)
    {
        for (int j = i + 1; j < n; j++)
        {
            if (WiFi.RSSI(indices[j]) > WiFi.RSSI(indices[i]))
            {
                std::swap(indices[i], indices[j]);
            }
        }
    }

    // 删除重复项（必须对RSSI进行排序）
    String cssid;
    for (int i = 0; i < n; i++)
    {
        if (indices[i] == -1)
            continue;
        cssid = WiFi.SSID(indices[i]);
        for (int j = i + 1; j < n; j++)
        {
            if (cssid == WiFi.SSID(indices[j]))
            {
                indices[j] = -1; // set dup aps to index -1
            }
        }
    }

    int _minimumQuality = -1;
    int quality;
    server->setContentLength(CONTENT_LENGTH_UNKNOWN);
    server->send_P(200, PSTR("text/html"), PSTR("{\"code\":1,\"msg\":\"\",\"data\":{\"list\":["));
    for (int i = 0; i < n; i++)
    {
        if (indices[i] == -1)
            continue; // skip dups
        int RSSI = WiFi.RSSI(indices[i]);
        if (RSSI <= -100)
        {
            quality = 0;
        }
        else if (RSSI >= -50)
        {
            quality = 100;
        }
        else
        {
            quality = 2 * (RSSI + 100);
        }
        if (_minimumQuality == -1 || _minimumQuality < quality)
        {
            if (i != 0)
            {
                server->sendContent_P(PSTR(","));
            }
            snprintf_P(tmpData, sizeof(tmpData), PSTR("{\"name\":\"%s\",\"rssi\":%d,\"quality\":%d,\"type\":%d}"), WiFi.SSID(indices[i]).c_str(), RSSI, quality, indices[i]);
            server->sendContent_P(tmpData);
        }
    }

    server->sendContent_P(PSTR("]}}"));
}

void Http::handleWifi()
{
    if (!checkAuth())
    {
        return;
    }
    String wifi = server->arg(F("wifi_ssid"));
    if (wifi == "")
    {
        server->send_P(200, PSTR("text/html"), PSTR("{\"code\":0,\"msg\":\"WiFi名称不能为空。\"}"));
        return;
    }
    String password = server->arg(F("wifi_password"));

    if (WiFi.getMode() == WIFI_STA)
    {
        strcpy(globalConfig.wifi.ssid, wifi.c_str());
        strcpy(globalConfig.wifi.pass, password.c_str());
        Config::saveConfig();
        server->send_P(200, PSTR("text/html"), PSTR("{\"code\":1,\"msg\":\"设置WiFi信息成功，重启模块（手动）使用新的Wifi信息连接。\"}"));
    }
    else
    {
        Wifi::tryConnect(wifi, password);
        server->send_P(200, PSTR("text/html"), PSTR("{\"code\":1,\"msg\":\"尝试将ESP连接到网络。 如果失败，请重新连接到AP再试一次。\"}"));
    }
}

void Http::handleOperate()
{
    if (!checkAuth())
    {
        return;
    }
    String d = server->arg(F("d")); // 1：重启模块 2：重置模块
    if (d == F("1"))
    {
        server->send_P(200, PSTR("text/html"), PSTR("{\"code\":1,\"msg\":\"设备正在重启 . . .\"}"));
    }
    else if (d == F("2"))
    {
        Config::resetConfig();
        Config::saveConfig();
        server->send_P(200, PSTR("text/html"), PSTR("{\"code\":1,\"msg\":\"正在重置模块 . . . 设备将会重启。\"}"));
    }
    delay(200);

    Led::blinkLED(400, 4);
    ESP.restart();
}

void Http::handleOTA()
{
    if (!checkAuth())
    {
        return;
    }
    String ota_url = server->arg(F("ota_url"));
    if (ota_url.length() > 149)
    {
        server->send_P(200, PSTR("text/html"), PSTR("{\"code\":0,\"msg\":\"OTA地址过长\"}"));
        return;
    }
    strcpy(globalConfig.http.ota_url, ota_url.c_str());
    server->send_P(200, PSTR("text/html"), PSTR("{\"code\":1,\"msg\":\"如果成功后设备会重启 . . . \"}"));
    Http::OTA(String(globalConfig.http.ota_url));
}

void Http::handleNotFound()
{
    if (captivePortal())
    {
        return;
    }
    // 配网模式 (configPortal 运行中): 对任意未匹配路径返回 200 + 自动跳转配网页面,
    // 而不是 404。系统 captive portal 检测请求 (generate_204 / hotspot-detect.html 等)
    // 无论携带域名还是 IP Host, 都能收到 200 + HTML 而被识别为门户, 提高自动弹窗兼容性
    if (Wifi::configPortalStart != 0)
    {
        server->send_P(200, PSTR("text/html"),
                       PSTR("<!DOCTYPE html><html><head><meta charset='utf-8'/>"
                            "<meta http-equiv='refresh' content='0;url=/'/>"
                            "<script>location.replace('/');</script>"
                            "</head><body>正在打开配网页面...</body></html>"));
        return;
    }
    // 避免未认证客户端通过 404 回显探测路由与请求参数
    if (!checkAuth())
    {
        return;
    }
    server->sendHeader(PSTR("Cache-Control"), PSTR("no-cache, no-store, must-revalidate"));
    server->sendHeader(PSTR("Pragma"), PSTR("no-cache"));
    server->sendHeader(PSTR("Expires"), PSTR("-1"));
    server->setContentLength(CONTENT_LENGTH_UNKNOWN);

    snprintf_P(tmpData, sizeof(tmpData), PSTR("File Not Found\n\nURI: %s\nMethod: %s\nArguments: %d\n"),
               server->uri().c_str(), server->method() == HTTP_GET ? PSTR("GET") : PSTR("POST"), server->args());
    server->send_P(404, PSTR("text/plain"), tmpData);
    for (uint8_t i = 0; i < server->args(); i++)
    {
        snprintf_P(tmpData, sizeof(tmpData), PSTR(" %s: %s\n"), server->argName(i).c_str(), server->arg(i).c_str());
        server->sendContent_P(tmpData);
    }
}

void Http::handleGetStatus()
{
    if (!checkAuth())
    {
        return;
    }

    server->setContentLength(CONTENT_LENGTH_UNKNOWN);
    server->send_P(200, PSTR("text/html"), PSTR("{\"code\":1,\"msg\":\"\",\"data\":{"));

    snprintf_P(tmpData, sizeof(tmpData), PSTR("\"uptime\":\"%s\",\"free_mem\":%d"), Rtc::msToHumanString(millis()).c_str(), ESP.getFreeHeap() / 1024);
    server->sendContent_P(tmpData);

    if (Wifi::configPortalStart == 0 && WiFi.isConnected())
    {
        snprintf_P(tmpData, sizeof(tmpData), PSTR(",\"ip\":\"%s\""), WiFi.localIP().toString().c_str());
        server->sendContent_P(tmpData);
    }

    if (module)
    {
        String tmp = module->httpGetStatus(server);
        if (tmp.length() > 0)
        {
            server->sendContent_P(PSTR(","));
            server->sendContent(tmp);
        }
    }

#ifdef WEB_LOG_SIZE
    bool cflg = true;
    uint8_t counter = 0;
    if (server->hasArg(F("i")))
    {
        counter = server->arg(F("i")).toInt();
    }
    snprintf_P(tmpData, sizeof(tmpData), PSTR(",\"logindex\":%d,\"log\":\""), Debug::webLogIndex);
    server->sendContent_P(tmpData);
    if (counter != Debug::webLogIndex)
    {
        if (!counter)
        {
            counter = Debug::webLogIndex;
            cflg = false;
        }
        do
        {
            char *tmp;
            uint16_t len;
            Debug::GetLog(counter, &tmp, &len);
            if (len)
            {
                if (cflg)
                {
                    server->sendContent_P(PSTR("\\n"));
                }

                size_t j = 0;
                // 转义后字节数可能超过 tmpData 容量 (日志含大量 \ 或 " 时最多膨胀一倍),
                // 必须限界, 防止写穿 512 字节缓冲区破坏相邻全局数据
                for (size_t i = 0; i < len - 1 && j < sizeof(tmpData) - 2; i++)
                {
                    char each = tmp[i];
                    if (each == '\\' || each == '"')
                    {
                        tmpData[j++] = '\\';
                        tmpData[j++] = each;
                    }
                    else if (each == '\b')
                    {
                        tmpData[j++] = '\\';
                        tmpData[j++] = 'b';
                    }
                    else if (each == '\f')
                    {
                        tmpData[j++] = '\\';
                        tmpData[j++] = 'f';
                    }
                    else if (each == '\n')
                    {
                        tmpData[j++] = '\\';
                        tmpData[j++] = 'n';
                    }
                    else if (each == '\r')
                    {
                        tmpData[j++] = '\\';
                        tmpData[j++] = 'r';
                    }
                    else if (each == '\t')
                    {
                        tmpData[j++] = '\\';
                        tmpData[j++] = 't';
                    }
                    else
                    {
                        tmpData[j++] = each;
                    }
                }
                tmpData[j++] = '\0';

                server->sendContent_P(tmpData);
                cflg = true;
            }
            counter++;
            if (!counter)
            {
                counter++;
            } // Skip log index 0 as it is not allowed
        } while (counter != Debug::webLogIndex);
    }
    server->sendContent_P(PSTR("\"}}"));
#else
    server->sendContent_P(PSTR("}}"));
#endif
}

void Http::handleUpdate()
{
    // handler for the /update form POST (once file upload finishes)
    // 上传认证失败标记用静态成员: lambda 注册后 handleUpdate() 即返回,
    // 局部变量会被 lambda 引用捕获造成悬垂引用(UB)
    server->on(
        F("/update"), HTTP_POST, [&]() {
        if (updateAuthFailed)
        {
            server->requestAuthentication();
            return;
        }
        if (!checkAuth())
        {
            return;
        } 
        if (Update.hasError())
        {
            uint8_t _error = Update.getError();
            if(_error == UPDATE_ERROR_WRITE){
                snprintf_P(tmpData, sizeof(tmpData), PSTR("Update Error[%u]: Flash Write Failed"), _error);
            } else if(_error == UPDATE_ERROR_ERASE){
                snprintf_P(tmpData, sizeof(tmpData), PSTR("Update Error[%u]: Flash Erase Failed"), _error);
            } else if(_error == UPDATE_ERROR_READ){
                snprintf_P(tmpData, sizeof(tmpData), PSTR("Update Error[%u]: Flash Read Failed"), _error);
            } else if(_error == UPDATE_ERROR_SPACE){
                snprintf_P(tmpData, sizeof(tmpData), PSTR("Update Error[%u]: Not Enough Space"), _error);
            } else if(_error == UPDATE_ERROR_SIZE){
                snprintf_P(tmpData, sizeof(tmpData), PSTR("Update Error[%u]: Bad Size Given"), _error);
            } else if(_error == UPDATE_ERROR_STREAM){
                snprintf_P(tmpData, sizeof(tmpData), PSTR("Update Error[%u]: Stream Read Timeout"), _error);
            } else if(_error == UPDATE_ERROR_SIGN){
                snprintf_P(tmpData, sizeof(tmpData), PSTR("Update Error[%u]: Signature verification failed"), _error);
            } else if(_error == UPDATE_ERROR_FLASH_CONFIG){
                snprintf_P(tmpData, sizeof(tmpData), PSTR("Update Error[%u]: Flash config wrong real: %d IDE: %d"), _error,  ESP.getFlashChipRealSize(), ESP.getFlashChipSize());
            } else if(_error == UPDATE_ERROR_NEW_FLASH_CONFIG){
                snprintf_P(tmpData, sizeof(tmpData), PSTR("Update Error[%u]: new Flash config wrong real: %d"), _error, ESP.getFlashChipRealSize());
            } else if(_error == UPDATE_ERROR_MAGIC_BYTE){
                snprintf_P(tmpData, sizeof(tmpData), PSTR("Update Error[%u]: Magic byte is wrong, not 0xE9"), _error);
            } else if (_error == UPDATE_ERROR_BOOTSTRAP){
                snprintf_P(tmpData, sizeof(tmpData), PSTR("Update Error[%u]: Invalid bootstrapping state, reset ESP8266 before updating"), _error);
            } else {
                snprintf_P(tmpData, sizeof(tmpData), PSTR("Update Error[%u]: UNKNOWN"), _error);
            }
            Debug::AddLog(LOG_LEVEL_ERROR);
            char out[150] = {0};
            snprintf_P(out, sizeof(out), PSTR("{\"code\":0,\"msg\":\"%s\"}"), tmpData);
            server->send_P(200, PSTR("text/html"), out);
        }
        else
        {
            Config::saveConfig();
            server->client().setNoDelay(true);
            server->send_P(200, PSTR("text/html"), PSTR("{\"code\":1,\"msg\":\"升级成功，设备将自动重启，稍后刷新页面即可。\"}"));
            //server->send_P(200, PSTR("text/html"), PSTR("<meta charset='utf-8'/><meta http-equiv=\"refresh\" content=\"15;URL=/\">升级成功！正在重启 . . ."));
            delay(100);
            server->client().stop();
            ESP.restart();
        } }, [&]() {
                HTTPUpload &upload = server->upload();
                if (upload.status == UPLOAD_FILE_START)
                {
                    // 每次上传开始重置认证失败标记, 避免上次失败残留导致后续认证成功的上传被误拒
                    updateAuthFailed = false;
                    if (globalConfig.http.user[0] != 0 && globalConfig.http.pass[0] != 0 && server->client().localIP().toString() != "192.168.4.1" && !server->authenticate(globalConfig.http.user, globalConfig.http.pass))
                    {
                        updateAuthFailed = true;
                        Debug::AddInfo(PSTR("Unauthenticated Update"));
                        return;
                    }
                    WiFiUDP::stopAll();
                    Debug::AddInfo(PSTR("Update: %s"), upload.filename.c_str());
                    uint32_t maxSketchSpace = (ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000;
                    if (!Update.begin(maxSketchSpace, U_FLASH))//start with max available size
                    { 
                    }
                }
                else if (upload.status == UPLOAD_FILE_WRITE && !Update.hasError() && !updateAuthFailed)
                {
                    if (Update.write(upload.buf, upload.currentSize) != upload.currentSize)
                    {
                    }
                }
                else if (upload.status == UPLOAD_FILE_END && !Update.hasError() && !updateAuthFailed)
                {
                    if (Update.end(true))
                    { 
                        Debug::AddInfo(PSTR("Update Success: %u   Rebooting..."), upload.totalSize);
                    }
                }
                else if (upload.status == UPLOAD_FILE_ABORTED)
                {
                    Update.end();
                    Debug::AddInfo(PSTR("Update was aborted"));
                }
                delay(0); });
}

void Http::begin()
{
    if (isBegin)
    {
        return;
    }
    isBegin = true;
    server = new ESP8266WebServer();

    server->on(F("/"), handleRoot);
    server->on(F("/dhcp"), handledhcp);
    server->on(F("/http"), handleHttp);
    server->on(F("/scan_wifi"), handleScanWifi);
    server->on(F("/wifi"), handleWifi);
    server->on(F("/operate"), handleOperate);
    server->on(F("/module_setting"), handleModuleSetting);
    server->on(F("/ota"), handleOTA);
    server->on(F("/get_status"), handleGetStatus);
    server->onNotFound(handleNotFound);
    handleUpdate();

    if (module)
    {
        module->httpAdd(server);
    }
    server->begin(globalConfig.http.port);
    Debug::AddInfo(PSTR("HTTP server started port: %d"), globalConfig.http.port);
}

void Http::stop()
{
    if (!isBegin)
    {
        return;
    }
    server->stop();
    Debug::AddInfo(PSTR("HTTP server stoped"));
}

void Http::loop()
{
    if (isBegin)
    {
        server->handleClient();
    }
}

bool Http::captivePortal()
{
    // 配网模式 (configPortal 运行中): 不重定向, 让根路径直接渲染配网页面 (200),
    // 系统 captive portal 检测收到 200 + HTML 比 302 更可靠地触发自动弹窗
    if (Wifi::configPortalStart != 0)
    {
        return false;
    }
    if (!Wifi::isIp(server->hostHeader()))
    {
        //Debug::AddInfo(PSTR("Request redirected to captive portal"));
        server->sendHeader(F("Location"), String(F("http://")) + server->client().localIP().toString(), true);
        server->send(302, F("text/plain"), ""); // Empty content inhibits Content-length header so we have to close the socket ourselves.
        server->client().stop();                // Stop is needed because we sent no content length
        return true;
    }
    return false;
}

void Http::handleModuleSetting()
{
    if (!checkAuth())
    {
        return;
    }

    int t = 0;
    if (server->arg(F("log_serial")).equals(F("1")))
    {
        t = t | 1;
    }
    if (server->arg(F("log_serial1")).equals(F("1")))
    {
        t = t | 8;
    }
#ifdef WEB_LOG_SIZE
    if (server->arg(F("log_web")).equals(F("1")))
    {
        t = t | 4;
    }
#endif

#ifdef USE_SYSLOG
    if (server->arg(F("log_syslog")).equals(F("1")))
    {
        t = t | 2;
        String log_syslog_host = server->arg(F("log_syslog_host"));
        String log_syslog_port = server->arg(F("log_syslog_port"));
        if (log_syslog_host.length() == 0)
        {
            server->send_P(200, PSTR("text/html"), PSTR("{\"code\":0,\"msg\":\"syslog服务器不能为空\"}"));
            return;
        }
        strcpy(globalConfig.debug.server, log_syslog_host.c_str());
        globalConfig.debug.port = log_syslog_port.toInt();
        WiFi.hostByName(globalConfig.debug.server, Debug::ip);
    }
#endif

    globalConfig.debug.type = t;
    if ((8 & globalConfig.debug.type) == 8)
    {
        Serial1.begin(115200);
    }

    String ntp = server->arg(F("ntp"));
    if (strcmp(globalConfig.wifi.ntp, ntp.c_str()) != 0)
    {
        strcpy(globalConfig.wifi.ntp, ntp.c_str());
        Rtc::init();
    }

    String uid = server->arg(F("uid"));
    strcpy(globalConfig.uid, uid.c_str());
    Config::saveConfig();
    if (uid.length() == 0 || strcmp(globalConfig.uid, UID) != 0)
    {
        server->send_P(200, PSTR("text/html"), PSTR("{\"code\":1,\"msg\":\"修改了重要配置 . . . 正在重启中。\"}"));
        Led::blinkLED(400, 4);
        ESP.restart();
    }
    else
    {
        server->send_P(200, PSTR("text/html"), PSTR("{\"code\":1,\"msg\":\"已经修改成功\"}"));
    }
}

bool Http::checkAuth()
{
    if (globalConfig.http.user[0] != 0 && globalConfig.http.pass[0] != 0 && server->client().localIP().toString() != F("192.168.4.1"))
    {
        if (!server->authenticate(globalConfig.http.user, globalConfig.http.pass))
        {
            server->requestAuthentication();
            return false;
        }
    }
    return true;
}

void Http::OTA(String url)
{
    if (url.indexOf(F("%04d")) != -1)
    {
        url.replace(F("%04d"), String(ESP.getChipId() & 0x1fff));
    }
    else if (url.indexOf(F("%d")) != -1)
    {
        url.replace(F("%d"), String(ESP.getChipId()));
    }
    url.replace(F("%hostname%"), UID);
    url.replace(F("%module%"), module ? module->getModuleName() : F(""));

    Config::saveConfig();
    Debug::AddInfo(PSTR("OTA Url: %s"), url.c_str());
    Led::blinkLED(200, 5);
    WiFiClient OTAclient;
    if (ESPhttpUpdate.update(OTAclient, url, (module ? module->getModuleVersion() : F(""))) == HTTP_UPDATE_FAILED)
    {
        Debug::AddError(PSTR("HTTP_UPDATE_FAILD Error (%d): %s"), ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
    }
}
