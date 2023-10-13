" Copyright 2023 Matthew Peter Smith

" This code is provided under an MIT License. 
" See LICENSE.txt at the root of this Git repository. 

function! EACInit()
    function! EACParse()
        let line_num = line('.')
        execute 'normal! ggVGd'
        execute 'read! eacy ' .. expand('%')
        execute 'normal! ggdd'
        execute 'normal! ' .. line_num .. 'ggzz'
    endfunction

    function! EACRebuild()
        execute "normal! ?#define EAC_LIB\rwww\"ryi\""
        let libname = getreg('r')
        execute "make " .. libname
    endfunction

    nnoremap <C-e> :call EACParse()<cr>
    nnoremap <C-r> :call EACRebuild()<cr>
endfunction

autocmd BufNewFile,BufReadPost *.c,*.cpp :call EACInit()
