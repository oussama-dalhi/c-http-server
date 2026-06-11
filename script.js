const urlInput = document.getElementById("urlInput");
const extractBtn = document.getElementById("extractBtn");
const output = document.getElementById("output");

const endPoint = "http://127.0.0.1:8000/transcript";

extractBtn.addEventListener("click", async () => {
  const url = urlInput.value;

  output.textContent = "Loading...";
  extractBtn.disabled = true;
  if (!url) {
  output.textContent = "Please enter a YouTube URL.";
  return;
}

  try {
    const response = await fetch(endPoint, {
      method: "POST",
      headers: {
        "Content-Type": "application/json"
      },
      body: JSON.stringify({
        url
      })
    });

    const data = await response.json();

    if (!data.success) {
      output.textContent = data.error;
      return;
    }
    output.textContent = data.transcript;
    urlInput.value = "";
  } catch (error) {
    console.error(error);
    output.textContent = "Failed to fetch transcript.";
  } finally {
    extractBtn.disabled = false;
  }
});