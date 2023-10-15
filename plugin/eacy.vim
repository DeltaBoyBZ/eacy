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
        execute "!mkdir -p .eac"
        execute "make .eac/" .. libname
    endfunction

    nnoremap <LocalLeader>ep :call EACParse()<cr>
    nnoremap <LocalLeader>er :call EACRebuild()<cr>
endfunction

autocmd BufNewFile,BufReadPost *.c,*.cpp :call EACInit()
