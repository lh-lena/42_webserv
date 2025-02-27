
const fileInput = document.getElementById('file');
const selectBtm = document.getElementById('uploadBtm');
const fileNameDisplay = document.getElementById('fileName');
const submitBtn = document.getElementById('submitBtn');

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

document.addEventListener("DOMContentLoaded", loadUploadedFiles);