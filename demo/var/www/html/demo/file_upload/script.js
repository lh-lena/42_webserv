
const fileInput = document.getElementById('file');
const selectBtm = document.getElementById('uploadBtm');
const fileNameDisplay = document.getElementById('fileName');
const submitBtn = document.getElementById('submitBtn');
const message = document.getElementById('message');

selectBtm.addEventListener('click', function () {
    fileInput.click();
});

fileInput.addEventListener('change', function () {
    if (fileInput.files.length > 0) {
        fileNameDisplay.textContent = fileInput.files[0].name;
    } else {
        fileNameDisplay.textContent = "No File Selected";
    }
});

submitBtn.addEventListener('click', function (e) {
    if (fileInput.files.length === 0) {
        e.preventDefault();
        message.style.display = 'block';
        message.innerHTML = "Please select a file before uploading";
        setTimeout( () => {
            message.style.display = 'none';
            message.innerHTML = "";
        }, 3000);
    }
    // else {
    //     window.location.reload();
    // }
});

// Fetch uploaded files and display them
// function loadUploadedFiles() {
//     fetch('list_files.php')
//         .then(response => response.json())
//         .then(data => {
//             const fileList = document.getElementById('file-list');
//             fileList.innerHTML = '';
//             data.forEach(file => {
//                 let li = document.createElement('li');
//                 li.textContent = file;
//                 fileList.appendChild(li);
//             });
//         });
// }

// document.addEventListener("DOMContentLoaded", loadUploadedFiles);