(defun mk/create-update-post ()
  "Create a new 'Update' post for today in the _posts folder."
  (interactive)
  (let* ((date (current-time))
         (month-name (downcase (format-time-string "%b" date)))
         (day (format-time-string "%d" date))
         (year (format-time-string "%Y" date))
         (full-month (format-time-string "%B" date))
         (weekday (format-time-string "%a" date))
         (time (format-time-string "%H:%M" date))
         (filename (format "%s_%s_%s.org" month-name day year))
         (project-root (locate-dominating-file default-directory ".git"))
         (posts-dir (expand-file-name "_posts" project-root))
         (filepath (expand-file-name filename posts-dir))
         (uuid (org-id-uuid))
         (org-date (format "<%s-%s-%s %s %s>" year (format-time-string "%m" date) day weekday time))
         (title (format "Update %s %s, %s" full-month day year)))

    ;; Check if file already exists
    (if (file-exists-p filepath)
        (message "Post file already exists: %s" filepath)

      ;; Create the file with the template
      (with-temp-file filepath
        (insert ":PROPERTIES:\n")
        (insert (format ":ID:       %s\n" uuid))
        (insert ":END:\n")
        (insert "\n")
        (insert (format "#+TITLE: %s\n" title))
        (insert (format "#+DATE: %s\n" org-date))
        (insert "#+filetags: :update:\n")
        (insert "\n")
        (insert "* What have I been up to?\n")
        (insert "\n"))

      ;; Open the file
      (find-file filepath)
      (goto-char (point-max))
      (message "Created new update post: %s" filepath))))

(defun mk/create-regular-post ()
  "Create a new regular blog post with a custom title in the _posts folder."
  (interactive)
  (let* ((title (read-string "Post title: "))
         (date (current-time))
         (year (format-time-string "%Y" date))
         (month (format-time-string "%m" date))
         (day (format-time-string "%d" date))
         (month-name (downcase (format-time-string "%b" date)))
         (weekday (format-time-string "%a" date))
         (time (format-time-string "%H:%M" date))
         (filename-base (replace-regexp-in-string "[^a-z0-9_]+" "_" (downcase title)))
         (filename (format "%s_%s_%s.org" month-name day year))
         (project-root (locate-dominating-file default-directory ".git"))
         (posts-dir (expand-file-name "_posts" project-root))
         (filepath (expand-file-name filename posts-dir))
         (uuid (org-id-uuid))
         (org-date (format "<%s-%s-%s %s %s>" year month day weekday time)))

    ;; Check if file already exists
    (if (file-exists-p filepath)
        (message "Post file already exists: %s" filepath)

      ;; Create the file with the template
      (with-temp-file filepath
        (insert ":PROPERTIES:\n")
        (insert (format ":ID:       %s\n" uuid))
        (insert ":END:\n")
        (insert (format "#+TITLE: %s\n" title))
        (insert (format "#+DATE: %s\n" org-date))
        (insert "#+filetags: :TODO:\n")
        (insert "\n")
        (insert "\n")
        (insert "\n")
        (insert "* Introduction\n")
        (insert ":PROPERTIES:\n")
        (insert (format ":ID:       %s\n" (org-id-uuid)))
        (insert ":END:\n")
        (insert "\n"))

      ;; Open the file
      (find-file filepath)
      (goto-char (point-max))
      (message "Created new regular post: %s" filepath))))

(provide 'create-update-post)
