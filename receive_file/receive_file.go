package main

import (
	"fmt"
	"io"
	"net/http"
	"os"
)

func main() {
	fmt.Println("TORKS field camera receiver");
	fmt.Println("Ready to receive file");
	http.HandleFunc("/upload", UploadFile)
	http.ListenAndServe(":8080", nil)
}

// UploadFile handles a POST request to upload a file
func UploadFile(w http.ResponseWriter, r *http.Request) {
	// Check request method
	if r.Method != "POST" {
		http.Error(w, "Invalid request method", http.StatusMethodNotAllowed)
		return
	}

	// Parse multipart form, 10 << 20 is the maxMemory limit. 
	// After exceeding this limit, the file will be saved in the temporary directory on the server.
	r.ParseMultipartForm(10 << 20)

	// Retrieve the file from form data
	file, header, err := r.FormFile("image")
	if err != nil {
		fmt.Println("Error retrieving the file")
		fmt.Println(err)
		return
	}
	defer file.Close()

	// Create a new file in the current directory
	dstFile, err := os.Create(header.Filename)
	if err != nil {
		fmt.Println(err)
		return
	}
	defer dstFile.Close()

	// Copy the uploaded file to the created file on the filesystem
	_, err = io.Copy(dstFile, file)
	if err != nil {
		fmt.Println(err)
		return
	}

	fmt.Printf("Uploaded File: %+v\n", header.Filename)
	fmt.Printf("File Size: %+v\n", header.Size)
//	fmt.Printf("MIME Header: %+v\n", header.Header)

	fmt.Fprintf(w, "Successfully Uploaded File\n")
}
