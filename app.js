let API = "";

const statusEl = document.getElementById("status");

const ipInput = document.getElementById("ip");

const savedIp = localStorage.getItem("arduino_ip");

if(savedIp){

  ipInput.value = savedIp;
}

async function conectar(){

  const ip = ipInput.value.trim();

  API = "http://" + ip;

  localStorage.setItem("arduino_ip", ip);

  statusEl.innerHTML = "conectando...";

  console.log(API);

  try{

    const r = await fetch(API + "/e", {

      method:"GET",

      mode:"cors"
    });

    const d = await r.json();

    console.log(d);

    statusEl.innerHTML = "conectado";

    status();

  }catch(e){

    console.log(e);

    statusEl.innerHTML = "erro";
  }
}

async function status(){

  if(!API) return;

  try{

    const r = await fetch(API + "/e", {

      method:"GET",

      mode:"cors"
    });

    const d = await r.json();

    if(d.perdeu){

      statusEl.innerHTML = "game over";

      return;
    }

    if(d.mostrando){

      statusEl.innerHTML = "observe";

      return;
    }

    if(d.esperando){

      statusEl.innerHTML = "sua vez";

      return;
    }

  }catch(e){

    console.log(e);

    statusEl.innerHTML = "offline";
  }
}

async function clicar(n){

  if(!API) return;

  try{

    const r = await fetch(
      API + "/btn?n=" + n,
      {
        method:"GET",
        mode:"cors"
      }
    );

    const d = await r.json();

    if(d.ok){

      statusEl.innerHTML = "acertou";

    }else{

      statusEl.innerHTML = "errou";
    }

  }catch(e){

    console.log(e);

    statusEl.innerHTML = "erro";
  }
}

async function reiniciar(){

  if(!API) return;

  try{

    await fetch(
      API + "/restart",
      {
        method:"GET",
        mode:"cors"
      }
    );

    statusEl.innerHTML = "reiniciado";

  }catch(e){

    console.log(e);

    statusEl.innerHTML = "erro";
  }
}

setInterval(status, 700);
