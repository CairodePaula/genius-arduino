let API = "";

const statusEl = document.getElementById("status");

const ipInput = document.getElementById("ip");

const savedIp = localStorage.getItem("arduino_ip");

if(savedIp){

  ipInput.value = savedIp;
}

async function conectar(){

  console.log("BOTAO CONNECT");

  const ip = ipInput.value.trim();

  if(!ip){

    statusEl.innerHTML = "digite IP";

    return;
  }

  API = "http://" + ip;

  localStorage.setItem("arduino_ip", ip);

  statusEl.innerHTML = "conectando...";

  console.log("API:", API);

  try{

    const r = await fetch(API + "/e", {

      method:"GET",

      mode:"cors"
    });

    const d = await r.json();

    console.log(d);

    statusEl.innerHTML = "conectado";

    setTimeout(status, 500);

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

  console.log("BOTAO:", n);

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

document
  .getElementById("connectBtn")
  .addEventListener("click", conectar);

document
  .getElementById("b0")
  .addEventListener("click", () => clicar(0));

document
  .getElementById("b1")
  .addEventListener("click", () => clicar(1));

document
  .getElementById("b2")
  .addEventListener("click", () => clicar(2));

document
  .getElementById("b3")
  .addEventListener("click", () => clicar(3));

document
  .getElementById("restartBtn")
  .addEventListener("click", reiniciar);

setInterval(status, 2500);
