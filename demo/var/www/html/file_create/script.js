
const uploadBtm = document.getElementById('uploadBtm');
const fileName = document.getElementById('fileName');
const fileBtn = document.getElementById('fileBody');

uploadBtm.addEventListener('click', function () {
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


/* Processing request...
request:
method = POST
reqURI = /file_create/index.html
fullPath = 
protocol = HTTP/1.1
query = 
upload path 
body = ------WebKitFormBoundaryKVlZSrodmpq2r3vB
Content-Disposition: form-data; name="file_name"

fdf
------WebKitFormBoundaryKVlZSrodmpq2r3vB
Content-Disposition: form-data; name="file_body"

fdfsdfsdf
------WebKitFormBoundaryKVlZSrodmpq2r3vB--

location: 
	** Location **
path: /
root: 
alias: 
autoindex: 1
clien max body: 0
upload dir: 
indexes: 
CGI extention: 
allowed methods: GET POST 
error pages: 
redirection: 

 */