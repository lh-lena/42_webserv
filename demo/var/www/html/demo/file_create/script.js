
const form = document.getElementById('file-form');

form.addEventListener('submit', async (e) => {
    e.preventDefault();

    const file_name = document.getElementById('fileName').value;
    const file_body = document.getElementById('fileBody').value;
    const message = document.getElementById('message');

    console.log("File name: ", file_name);

    const req_url = window.location.protocol + '//' + window.location.host + '/create_file.php';

    // send data to the server
    try {
        const response = await fetch(req_url, {
            method: "POST",
            body: JSON.stringify({ file_name, file_body }),
        });
        if (!response.ok) {
            throw new Error(`Response status: ${response.status}`);
        }
        const json = await response.json();
        message.innerHTML = "File " + file_name + " saved successfully!";
        setTimeout(() => {
            form.reset();
            message.innerHTML = "";
            console.log(json.message);
        }), 10000;
    } catch (error) {
        form.reset();
        console.error(error.message);
    }
});
