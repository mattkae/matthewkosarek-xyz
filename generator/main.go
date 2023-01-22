package main

import "fmt"

/*

   Form of the static site generator:
   1. Define a list of static items
   2. Define a global index.css
   3. Each static site item has a content.html file
   4. Each static site item has a content.css file
   5. Each static site item can have a content.js file
   6. Each static site item can have an optional build
   7. There will be some built-in blogging functionality if we do it right
*/

func main() {
	doWork()
}

func doWork() {
	fmt.Println("Hello, world!")
	
}
