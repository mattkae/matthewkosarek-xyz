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

;; Install rust-mode if not already installed
(unless (package-installed-p 'rust-mode)
  (package-refresh-contents)
  (package-install 'rust-mode))

(defun get-org-file-title(entry style project)
  (setq timestamp (org-timestamp-format (car (org-publish-find-property entry :date project)) "%B %d, %Y"))
  (setq tag-list (org-publish-find-property entry :filetags project))
  (setq tag-list-str (mapconcat 'identity tag-list ","))
  (setq result (format "%s\n #+begin_sitemap_date\n%s\n#+end_sitemap_date\n#+begin_sitemap_tag\n%s\n#+end_sitemap_tag\n" (org-publish-sitemap-default-entry entry style project) timestamp tag-list-str))
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
             :publishing-function 'my-org-html-publish-to-html
             :with-toc nil
             :with-author nil
             :section-numbers nil
             :time-stamp-file nil
             :with-title nil
             :with-date nil
             :html-preamble-format '(("en" "
  <div class=\"org-article-title\">
    <h1>%t</h1>
    <a href=\"/posts/feed.xml\">RSS Feed</a>
  </div>
"))
             :auto-sitemap t
             :sitemap-sort-files 'anti-chronologically
             :sitemap-title ""
             :sitemap-format-entry (lambda (entry style project) (get-org-file-title entry style project))
             :sitemap-function (lambda (title list) (my-sitemap-function title list))
             )))


(setq org-html-htmlize-output-type 'css)
(setq org-html-htmlize-font-prefix "org-")

(defun my-org-html-publish-to-html (plist filename pub-dir)
  "Custom HTML publish function that conditionally adds sitemap.css."
  (let* ((is-sitemap (string-match-p "sitemap\\.org$" filename))
         (org-html-head-extra (when is-sitemap
                                "<link rel=\"stylesheet\" href=\"/posts/sitemap.css\" />")))
    (org-html-publish-to-html plist filename pub-dir)))

(setq org-html-validation-link nil
      org-html-head-include-scripts nil       ;; Use our own scripts
      org-html-head-include-default-style nil ;; Use our own styles
      org-html-head "
<link rel=\"stylesheet\" href=\"/index.css\" />
<link rel=\"stylesheet\" href=\"/posts/post.css\" />
<link rel=\"shortcut icon\" href=\"/favicon/favicon.ico\" type=\"image/x-icon\">
"
      org-html-inline-images t
      org-html-link-home "/"
      org-html-link-up "/posts/sitemap.html"
      org-html-html5-fancy t
      org-html-home/up-format "<header>
  <nav>
    <ul>
      <li><a href='/'>&#127969; Home</a></li>
      <li><a href='/resume.html'>&#128216; CV</a></li>
      <li><a href='/posts/sitemap.html'>&#128221; Posts</a></li>
    </ul>
  </nav>
</header>"
      )

(defun update-rss-feed ()
  "Update the RSS feed with the newest post from _posts directory."
  (let* ((posts-dir (expand-file-name "_posts"))
         (feed-file (expand-file-name "posts/feed.xml"))
         (org-files (seq-filter (lambda (f) (not (string-match-p "sitemap\\.org$" f)))
                                (directory-files posts-dir t "\\.org$")))
         (newest-file (car (sort org-files (lambda (a b) (time-less-p (file-attribute-modification-time (file-attributes b))
                                                                        (file-attribute-modification-time (file-attributes a)))))))
         title date-str filename-base)

    (when newest-file
      ;; Parse the newest org file
      (with-temp-buffer
        (insert-file-contents newest-file)
        (goto-char (point-min))

        ;; Extract title
        (when (re-search-forward "^#\\+TITLE:\\s-*\\(.+\\)$" nil t)
          (setq title (match-string 1)))

        ;; Extract date
        (goto-char (point-min))
        (when (re-search-forward "^#\\+DATE:\\s-*<\\([0-9]\\{4\\}\\)-\\([0-9]\\{2\\}\\)-\\([0-9]\\{2\\}\\)\\s-+\\w+\\s-+\\([0-9]\\{2\\}\\):\\([0-9]\\{2\\}\\)>" nil t)
          (let ((year (match-string 1))
                (month (match-string 2))
                (day (match-string 3))
                (hour (match-string 4))
                (minute (match-string 5))
                (month-names ["" "January" "February" "March" "April" "May" "June"
                             "July" "August" "September" "October" "November" "December"])
                (day-names ["Sun" "Mon" "Tue" "Wed" "Thu" "Fri" "Sat"]))
            (setq date-str (format "%s, %s %s %s %s:%s:00 -0400"
                                   (aref day-names (string-to-number (format-time-string "%w" (encode-time 0 0 0 (string-to-number day) (string-to-number month) (string-to-number year)))))
                                   day
                                   (aref month-names (string-to-number month))
                                   year
                                   hour
                                   minute)))))

      ;; Get filename without extension
      (setq filename-base (file-name-sans-extension (file-name-nondirectory newest-file)))

      ;; Create RSS item
      (when (and title date-str filename-base)
        (let ((rss-item (format "    <item>
      <title>%s</title>
      <link>https://matthewkosarek.xyz/posts/%s.html</link>
      <guid>https://matthewkosarek.xyz/posts/%s.html</guid>
      <pubDate>%s</pubDate>
      <description>%s</description>
    </item>\n"
                                title filename-base filename-base date-str title)))

          ;; Read current feed
          (with-temp-buffer
            (insert-file-contents feed-file)
            (goto-char (point-min))

            ;; Update lastBuildDate to current time
            (let* ((current-time (current-time))
                   (decoded-time (decode-time current-time))
                   (day (nth 3 decoded-time))
                   (month (nth 4 decoded-time))
                   (year (nth 5 decoded-time))
                   (hour (nth 2 decoded-time))
                   (minute (nth 1 decoded-time))
                   (month-names ["" "January" "February" "March" "April" "May" "June"
                                "July" "August" "September" "October" "November" "December"])
                   (day-names ["Sun" "Mon" "Tue" "Wed" "Thu" "Fri" "Sat"])
                   (current-date-str (format "%s, %02d %s %s %02d:%02d:00 -0400"
                                            (aref day-names (nth 6 decoded-time))
                                            day
                                            (aref month-names month)
                                            year
                                            hour
                                            minute)))
              (when (re-search-forward "<lastBuildDate>\\(.*\\)</lastBuildDate>" nil t)
                (replace-match (format "<lastBuildDate>%s</lastBuildDate>" current-date-str))))

            ;; Check if this item already exists
            (goto-char (point-min))
            (unless (search-forward (format "<link>https://matthewkosarek.xyz/posts/%s.html</link>" filename-base) nil t)
              ;; Insert new item after lastBuildDate
              (goto-char (point-min))
              (when (re-search-forward "<lastBuildDate>.*</lastBuildDate>\n" nil t)
                (insert "\n")
                (insert rss-item)
                (message "Added RSS item for: %s" title)))

            ;; Write updated feed
            (write-region (point-min) (point-max) feed-file)))))))

(org-publish-all t)

;; Update RSS feed after publishing
(update-rss-feed)

(message "Build Complete")
