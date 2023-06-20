(require 'ox-publish)

(require 'package)
(setq package-user-dir (expand-file-name "./.packages"))
(setq package-archives '(("melpa" . "https://melpa.org/packages/")
                         ("elpa" . "https://elpa.gnu.org/packages/")))

;; Initialize the package system
(package-initialize)
(unless package-archive-contents
  (package-refresh-contents))

;; Install dependencies
(package-install 'htmlize)

(defun get-org-file-title(entry style project)
  (setq timestamp (org-timestamp-format (car (org-publish-find-property entry :date project)) "%B %d, %Y"))
  (setq tag-list (org-publish-find-property entry :filetags project))
  (setq tag-list-str (mapconcat 'identity tag-list ","))
  (setq result (format "%s created on %s\n#+begin_sitemap_tag\n%s\n#+end_sitemap_tag\n" (org-publish-sitemap-default-entry entry style project) timestamp tag-list-str))
  )

(defun my-sitemap-function (title list)
  (concat "#+TITLE: " title "\n\n"
          "#+DATE: " (format-time-string "%Y-%m-%d at %H:%M") "\n\n"
          "#+HTML_LINK_HOME: /\n\n"
          "#+HTML_LINK_UP: /\n\n"
	        (org-list-to-org list)))

(setq org-publish-project-alist
      (list
       (list "matthewkosarek.xyz"
             :recursive t
             :base-directory "./_posts"
             :publishing-directory "./posts"
             :publishing-function: 'org-html-publish-to-html
             :with-toc nil
             :with-author nil
             :section-numbers nil
             :time-stamp-file nil
             :with-title nil
             :with-date nil
             :html-preamble-format '(("en" "
  <div class=\"org-article-title\">
    <h1>%t</h1>
    <span>Last modified: %d</span>
  </div>
"))
             :auto-sitemap t
             :sitemap-sort-files "chronologically"
             :sitemap-title "Matthew's Blog Posts"
             :sitemap-format-entry (lambda (entry style project) (get-org-file-title entry style project))
             :sitemap-function (lambda (title list) (my-sitemap-function title list))
             )))


(setq org-html-htmlize-output-type 'css)
(setq org-html-htmlize-font-prefix "org-")

(setq org-html-validation-link nil
      org-html-head-include-scripts nil       ;; Use our own scripts
      org-html-head-include-default-style nil ;; Use our own styles
      org-html-head "
<link rel=\"stylesheet\" href=\"/index.css\" />
<link rel=\"stylesheet\" href=\"/posts/post.css\" />
<link rel=\"shortcut icon\" href=\"/favicon/favicon.ico\" type=\"image/x-icon\">
<script src='/posts/post.js'></script>
"
      org-html-inline-images t
      org-html-link-home "/"
      org-html-link-up "/posts/sitemap.html"
      org-html-html5-fancy t
      org-html-home/up-format "<div id=\"org-div-home-and-up\">
 <a accesskey=\"h\" href=\"%s\"> UP </a>
 <a accesskey=\"H\" href=\"%s\"> HOME </a>
</div>"
      )

(org-publish-all t)

(message "Build Complete")
