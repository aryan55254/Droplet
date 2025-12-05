import net from "net";

const HOST = "localhost";
const PORT = 8080;

const TOTAL_REQUESTS = 20000;
const CONCURRENCY = 200;

let sent = 0;
let done = 0;
let allowed = 0;
let limited = 0;

console.log(
  `Sending ${TOTAL_REQUESTS} raw TCP requests with concurrency ${CONCURRENCY}`
);

const start = Date.now();

function sendOne() {
  if (sent >= TOTAL_REQUESTS) return;
  sent++;

  const client = new net.Socket();

  client.connect(PORT, HOST, () => {
    client.write("PING\n");
  });

  client.on("data", (data) => {
    const msg = data.toString().trim();
    if (msg === "OK") allowed++;
    else limited++;

    client.destroy(); // close socket
    done++;

    if (sent < TOTAL_REQUESTS) sendOne();
    if (done === TOTAL_REQUESTS) finish();
  });

  client.on("error", () => {
    limited++;
    done++;
    if (sent < TOTAL_REQUESTS) sendOne();
    if (done === TOTAL_REQUESTS) finish();
  });
}

for (let i = 0; i < CONCURRENCY; i++) sendOne();

function finish() {
  const duration = (Date.now() - start) / 1000;
  console.log(`\n=== Test Complete ===`);
  console.log(`Allowed:      ${allowed}`);
  console.log(`Rate Limited: ${limited}`);
  console.log(`Time:         ${duration.toFixed(2)}s`);
  console.log(`Req/sec:      ${(TOTAL_REQUESTS / duration).toFixed(0)}`);
}
