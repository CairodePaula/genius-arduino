let API = "";
async function status() {

  if(!API) return;

  try {

    const r = await fetch(API + "/e");

    const d = await r.json();

    if(d.perdeu) {

      statusEl.innerHTML = "game over";

      return;
    }

    if(d.mostrando) {

      statusEl.innerHTML = "observe";

      return;
    }

    if(d.esperando) {

      statusEl.innerHTML = "sua vez";

      return;
    }

  } catch(e) {

    statusEl.innerHTML = "offline";
  }
}

async function clicar(n) {

  if(!API) return;

  try {

    const r = await fetch(API + "/btn?n=" + n);

    const d = await r.json();

    if(d.ok) {

      statusEl.innerHTML = "acertou";

    } else {

      statusEl.innerHTML = "errou";
    }

  } catch(e) {

    statusEl.innerHTML = "erro";
  }
}

async function reiniciar() {

  if(!API) return;

  try {

    await fetch(API + "/restart");

    statusEl.innerHTML = "reiniciado";

  } catch(e) {

    statusEl.innerHTML = "erro";
  }
}

setInterval(status, 700);
