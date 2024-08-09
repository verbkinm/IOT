var socket = new WebSocket("ws://192.168.4.1/ws");

socket.onopen = function(e) {
    // alert("[open] Соединение установлено");
    // alert("Отправляем данные на сервер");

    socket.send(JSON.stringify(msg));
};

socket.onmessage = function(event) {
    // alert(`[message] Данные получены с сервера: ${event.data}`);
    var obj = JSON.parse(event.data);
    console.log(obj);

    switch (obj.type) {
    case "sysinf":
      document.getElementById("sysinf").innerHTML = replace_slash(obj.text)
      break;
    case "sta_state":
      var state = document.getElementById("sta_state")
      state.innerHTML = obj.state
      if (state.innerHTML == "online")
          state.style.color = "green"
      else if (state.innerHTML == "offline")
          state.style.color = "red"

      document.getElementById("ssid").value = obj.ssid
      document.getElementById("pwd").value = obj.pwd

      if (obj.state == "connecting")
          modal_msg("Подождите...")
      else
      {
          modal_msg_hide()
          clearInterval(timerId);
      }
      break;
    case "scan_result":
        var select = document.getElementById("select");
        select.innerHTML = "";

        obj.result.forEach ((element, index, array) => {
            var option = document.createElement("option");
            option.text = option.text.concat(index + 1, " - \"", element[0], "\" (", element[1], ")");
            option.value = element[0];

            select.add(option);
        })

        modal_msg_hide()
      break;
    case "ota":
        if (obj.url)
            document.getElementById("url").value = obj.url;

        if (obj.result)
        {
            if (obj.result == "ok")
                modal_msg("Перезагрузите устройство.")
            else if (obj.result == "err")
                modal_msg("Ошибка обновления!", 3000);
        }

    break;
  }

  // socket.close();
};

socket.onclose = function(event) {
    if (event.wasClean) {
        console.log(`[close] Соединение закрыто чисто, код=${event.code} причина=${event.reason}`);
    } else {
        // например, сервер убил процесс или сеть недоступна
        // обычно в этом случае event.code 1006
        console.log('[close] Соединение прервано');
    }
};

socket.onerror = function(error) {
    // alert(`[error]`);
};

function replace_slash(str) {
    var re = /\\\"/gi
    return str.replace(re, "\"");
}

function modal_msg(text, interval) {
    document.getElementById("modal").style.display = "block";
    document.getElementById("modal_text").innerHTML = text;

    if (interval)
        setTimeout(modal_msg_hide, interval);
}

function modal_msg_hide() {
    document.getElementById("modal").style.display = "none";
}
