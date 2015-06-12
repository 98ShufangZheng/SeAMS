% EQUALISECOLOURMAP - Equalise colour contrast over a colourmap
%
% Usage: newrgbmap = equalisecolourmap(rgblab, map, formula, W, sigma, diagnostics)
%
% Arguments:     rgblab - String 'RGB' or 'LAB' indicating the type of data
%                         in map.
%                   map - A Nx3 RGB or CIELAB colour map
%               formula - String 'CIE76' or 'CIEDE2000'
%                     W - A 3-vector of weights to be applied to the
%                         lightness, chroma and hue components of the
%                         difference equation. Use [1 0 0] to only take into
%                         account lightness, [1 1 1] for the full formula.
%                         See note below.
%                 sigma - Optional Gaussian smoothing parameter, see
%                         explanation below.
%                cyclic - Flag 1/0 indicating whether the colour map is
%                         cyclic. This affects how smoothing is applied at
%                         the end points.
%           diagnostics - Optional flag 0/1 indicating whether diagnostic
%                         plots should be displayed.  Defaults to 0.
%
% Returns:    newrgbmap - RGB colour map adjusted so that the perceptual
%                         contrast of colours along the colour map is constant.
%
% Suggested parameters:
%
% The CIE76 and CIEDE2000 colour difference formulas were developed for much
% lower spatial frequencies than we are typically interested in.  Neither is
% ideal for our application.  While the CIEDE2000 lightness correction is
% probably useful the difference is barely noticeable. The CIEDE2000 chroma
% correction is definitely *not* suitable for our needs. 
%
% For colour maps with a significant range of lightness use:
%                        formula = 'CIE76' or 'CIEDE2000'
%                              W = [1 0 0]  (Only correct for lightness)
%                          sigma = 5 - 7
%
% For isoluminant or low lightness gradient colour maps use:  
%                        formula = 'CIE76'
%                              W = [1 1 1]  (Correct for colour and lightness)
%                          sigma = 5 - 7
%
% Ideally, for a colour map to be effective the perceptual contrast along the
% colour map should be constant.  Many colour maps are very poor in this regard.
% Try testing your favourite colour map on the SINERAMP test image.  The
% perceptual contrast is very much dominated by the contrast in colour lightness
% values along the map.  This function attempts to equalise the chosen
% perceptual contrast measure along a colour map by stretching and/or
% compressing sections of the colour map.
%
% This function's primary use is for the correction of colour maps generated by
% CMAP however it can be applied to any colour map.  There are limitations to
% what this function can correct.  When applied to some of MATLAB's colour maps
% such as 'jet', 'hsv' and 'cool' you get colour discontinuity artifacts because
% these colour maps have segments that are nearly constant in lightness.
% However, it does a nice job of fixing up MATLAB's 'hot', 'winter', 'spring'
% and 'autumn' colour maps.  If you do see colour discontinuities in the
% resulting colour map try changing W from [1 0 0] to [1 1 1], or some
% intermediate weighting of [1 0.5 0.5], say.
%
% Difference formula: Neither CIE76 or CIEDE2000 difference measures are ideal
% for the high spatial frequencies that we are interested in.  Empirically I
% find that CIEDE2000 seems to give slightly better results on colour maps where
% there is a significant lightness gradient (this applies to most colour maps).
% In this case you would be using a weighting vector W = [1 0 0].  For
% isoluminant, or low lightness gradient colour maps where one is using a
% weighting vector W = [1 1 1] CIE76 should be used as the CIEDE2000 chroma
% correction is inapropriate for the spatial frequencies we are interested in.
%
% Weighting vetor W: The CIEDE2000 colour difference formula incorporates the
% scaling parameters kL, kC, kH in the demonimator of the lightness, chroma, and
% hue difference components respectively.  The 3 components of W correspond to
% the reciprocal of these 3 parameters.  (I do not know why they chose to put
% kL, kC, kH in the denominator. If you wanted to ignore, say, the chroma
% component you would have to set kC to Inf, rather than setting W(2) to 0 which
% seems more sensible to me).  If you are using CIE76 then W(2) amd W(3) are
% applied to the differences in a and b.  In this case you should ensure W(2) =
% W(3).  In general, for the spatial frequencies of interest to us, lightness
% differences are overwhelmingly more important than chroma or hue and W shoud
% be set to [1 0 0]
%
% Smoothing parameter sigma: 
% The output colour map will have lightness values of constant slope magnitude.
% However, it is possible that the sign of the slope may change, for example at
% the mid point of a bilateral colour map.  This slope discontinuity of lightness
% can induce a false apparent feature in the colour map.  A smaller effect is
% also occurs for slope discontinuities in a and b.  For such colour maps it can
% be useful to introduce a small amount of smoothing of the Lab values to soften
% the transition of sign in the slope to remove this apparent feature.  However
% in doing this one creates a small region of suppressed luminance contrast in
% the colour map which induces a 'blind spot' that compromises the visibility of
% features should they fall in that data range.  Accordingly the smoothing
% should be kept to a minimum.  A value of sigma in the range 5 to 7 in a 256
% element colour map seems about right.  As a guideline sigma should not be more
% than about 1/25 of the number of entries in the colour map, preferably less.
%
% If you use the CIEDE2000 formula option this function requires Gaurav Sharma's
% MATLAB implementation of the CIEDE2000 color difference formula deltaE2000.m
% available at: http://www.ece.rochester.edu/~gsharma/ciede2000/
%
% See also: CMAP, SINERAMP, COLOURMAPPATH, ISOCOLOURMAPPATH

% Copyright (C) 2013-2014 Peter Kovesi
% Centre for Exploration Targeting
% The University of Western Australia
% peter.kovesi at uwa edu au
% 
% Permission is hereby granted, free of charge, to any person obtaining a copy
% of this software and associated documentation files (the "Software"), to deal
% in the Software without restriction, subject to the following conditions:
% 
% The above copyright notice and this permission notice shall be included in 
% all copies or substantial portions of the Software.
%
% The Software is provided "as is", without warranty of any kind.
%
% December 2013  - Original version
% June     2014  - Generalised and cleaned up

function newrgbmap = equalisecolourmap(rgblab, map, formula, W, sigma, cyclic, diagnostics)
    
    if ~exist('formula', 'var'), formula = 'CIE76'; end
    if ~exist('W', 'var'), W = [1 0 0]; end
    if ~exist('sigma', 'var'), sigma = 0; end
    if ~exist('cyclic', 'var'), cyclic = 0; end
    if ~exist('diagnostics', 'var'), diagnostics = 0; end
    
    N = size(map,1);  % No of colour map entries

    if N/sigma < 25
        warning(['It is not recommended that sigma be larger than 1/25 of' ...
                 ' colour map length'])
    end

    if strcmpi(rgblab, 'RGB') & (max(map(:)) > 1.01  | min(map(:) < 0.01))
        error('If map is RGB values should be in the range 0-1')
    elseif strcmpi(rgblab, 'LAB') & max(abs(map(:))) < 10
        error('If map is LAB magnitude of values are expected to be > 10')
    end
    
    % Use D65 whitepoint to match typical monitors.
    wp = whitepoint('D65');
    
    % If input is RGB convert colour map to Lab. 
    if strcmpi(rgblab, 'RGB')
        rgbmap = map;
        labmap = applycform(map, makecform('srgb2lab', 'AdaptedWhitePoint', wp));
        L = labmap(:,1);
        a = labmap(:,2);
        b = labmap(:,3);
    elseif strcmpi(rgblab, 'LAB')
        labmap = map;
        rgbmap = applycform(map, makecform('lab2srgb', 'AdaptedWhitePoint', wp));
        L = map(:,1);
        a = map(:,2);
        b = map(:,3);
    else
        error('Input must be RGB or LAB')
    end

    % The following section of code computes the locations to interpolate into
    % the colour map in order to achieve equal steps of perceptual contrast.
    % The process is repeated recursively on its own output. This helps overcome
    % the approximations induced by using linear interpolation to estimate the
    % locations of equal perceptual contrast. This is mainly an issue for
    % colour maps with only a few entries.

    for iter = 1:3
        % Compute perceptual colour difference values along the colour map using
        % the chosen formula and weighting vector.
        if strcmpi(formula, 'CIE76')
            deltaE = cie76(L, a, b, W);
        elseif strcmpi(formula, 'CIEDE2000')
            if ~exist('deltaE2000','file')
                fprintf('You need Gaurav Sharma''s function deltaE2000.m\n');
                fprintf('available at:\n');
                fprintf('http://www.ece.rochester.edu/~gsharma/ciede2000/\n');
                newrgbmap = [];
                return
            end
                
            deltaE = ciede2000(L, a, b, W);
        else
            error('Unknown colour difference formula')
        end
        
        % Form cumulative sum of of delta E values.  However, first ensure all
        % values are larger than 0.001 to ensure the cumulative sum always
        % increases.
        deltaE(deltaE < 0.001) = 0.001;
        cumdE = cumsum(deltaE);
        
        % Form an array of equal steps in cumulative contrast change.
        equicumdE =  (0:(N-1))'/(N-1) * (cumdE(end)-cumdE(1)) + cumdE(1);
        
        % Solve for the locations that would give equal Delta E values.
        method = 'linear';
        newN = interp1(cumdE, (1:N)', equicumdE, method, 'extrap');
        
        % newN now represents the locations where we want to interpolate into the
        % colour map to obtain constant perceptual contrast
        L = interp1((1:N)', L, newN, method, 'extrap');  
        a = interp1((1:N)', a, newN, method, 'extrap');
        b = interp1((1:N)', b, newN, method, 'extrap');
    
        % Record initial colour differences for evaluation at the end
        if iter == 1  
            initialdeltaE = deltaE;
            initialcumdE = cumdE;
            initialequicumdE = equicumdE;
            initialnewN = newN;
        end
    end
    
    % Apply smoothing of the path in CIELAB space if requested.  The aim is to
    % smooth out sharp lightness/colour changes that might induce the perception
    % of false features.  In doing this there will be some cost to the
    % perceptual contrast at these points.
    if sigma
        L = smooth(L, sigma, cyclic);
        a = smooth(a, sigma, cyclic);
        b = smooth(b, sigma, cyclic);
    end

    % Convert map back to RGB
    newlabmap = [L a b];
    newrgbmap = applycform(newlabmap, makecform('lab2srgb', 'AdaptedWhitePoint', wp));
    
    if diagnostics
        % Compute actual perceptual contrast values achieved
        if strcmpi(formula, 'CIE76')
            newdeltaE = cie76(L, a, b, W);
        elseif strcmpi(formula, 'CIEDE2000')
            newdeltaE = ciede2000(L, a, b, W);
        else
            error('Unknown colour difference formula')
        end
    
        show(sineramp,1,'Unequalised input colour map'), colormap(rgbmap);
        show(sineramp,2,'Equalised colour map'), colormap(newrgbmap);

        figure(3), clf
        subplot(2,1,1)
        plot(initialcumdE)
        title(sprintf('Cumulative change in %s of input colour map', formula));
        axis([1 N 0 1.05*max(cumdE(:))]);
        xlabel('Colour map index');

        subplot(2,1,2)
        plot(1:N, initialdeltaE, 1:N, newdeltaE)
        axis([1 N 0 1.05*max(initialdeltaE(:))]);
        legend('Original colour map', ...
               'Adjusted colour map', 'location', 'NorthWest');
        title(sprintf('Magnitude of %s differences along colour map', formula));
        xlabel('Colour map index');
        ylabel('dE')
    
        % Convert newmap back to Lab to check for gamut clipping
        labmap = applycform(newrgbmap,...
                            makecform('srgb2lab', 'AdaptedWhitePoint', wp));
        figure(4), clf
        subplot(3,1,3)
        plot(1:N, L-labmap(:,1),...
             1:N, a-labmap(:,2),...
             1:N, b-labmap(:,3))
        legend('Lightness', 'a', 'b', 'Location', 'NorthWest')
        maxe = max(abs([L-labmap(:,1); a-labmap(:,2); b-labmap(:,3)]));
        axis([1 N -maxe maxe]);
        title('Difference between desired and achieved L a b values (gamut clipping)')
        
        % Plot RGB values
        subplot(3,1,1)
        plot(1:N, newrgbmap(:,1),'r-',...
             1:N, newrgbmap(:,2),'g-',...
             1:N, newrgbmap(:,3),'b-')
    
        legend('Red', 'Green', 'Blue', 'Location', 'NorthWest')
        axis([1 N 0 1.1]);        
        title('RGB values along colour map')
        
        % Plot Lab values
        subplot(3,1,2)
        plot(1:N, L, 1:N, a, 1:N, b)
        legend('Lightness', 'a', 'b', 'Location', 'NorthWest')
        axis([1 N -100 100]);                
        title('L a b values along colour map')
    
        
        %% Extra plots to generate figures.
        % The following plots were developed for illustrating the
        % equalization of MATLAB's hot(64) colour map.  Run using:
        % >> hoteq = equalisecolourmap('RGB', hot, 'CIE76', [1 0 0], 0, 0, 1);

        fs = 12; % fontsize
        % Colour difference values
        figure(10), clf
        plot(1:N, initialdeltaE, 'linewidth', 2);
        axis([1 N 0 1.05*max(initialdeltaE(:))]);
        title(sprintf('Magnitude of %s lightness differences along colour map', ...
                      formula), 'fontweight', 'bold', 'fontsize', fs);
        
        xlabel('Colour map index', 'fontweight', 'bold', 'fontsize', fs);
        ylabel('dE', 'fontweight', 'bold', 'fontsize', fs);

        % Cumulative difference plot showing mapping of equicontrast colours
        figure(11), clf
        plot(initialcumdE, 'linewidth', 2), hold on
        s = 7;
        for n = 1:s:N
            line([0 initialnewN(n) initialnewN(n)],...
                 [initialequicumdE(n) initialequicumdE(n) 0], ...
                 'linestyle', '--', 'color', [0 0 0])
 
            ah = s/2; % Arrow height and width
            aw = ah/5;
            plot([initialnewN(n)-aw initialnewN(n) initialnewN(n)+aw],...
                 [ah 0 ah],'k-')            
        end
        
        title(sprintf('Cumulative change in %s lightness of colour map', formula), ...
              'fontweight', 'bold', 'fontsize', fs);
        axis([1 N+6 0 1.05*max(cumdE(:))]);
        xlabel('Colour map index', 'fontweight', 'bold', 'fontsize', fs);
        ylabel('Equispaced cumulative contrast', 'fontweight', 'bold', 'fontsize', fs);
        hold off
        
        
    end

%----------------------------------------------------------------------------
%
% Function to smooth an array of values but also ensure end values are not
% altered or, if the map is cyclic, ensures smoothing is applied across the end
% points in a cyclic manner.  Assumed that the input data is a column vector

function Ls = smooth(L, sigma, cyclic)
    
    sze = ceil(6*sigma);  if ~mod(sze,2), sze = sze+1; end
    G = fspecial('gaussian', [sze 1], sigma);
    
    if cyclic 
        Le = [L(:); L(:); L(:)]; % Form a concatenation of 3 repetitions of the array. 
        Ls = filter2(G, Le);     % Apply smoothing filter
                                 % and then return the center section    
        Ls = Ls(length(L)+1: length(L)+length(L));
    
    else  % Non-cyclic colour map: Pad out input array L at both ends by 3*sigma
          % with additional values at the same slope.  The aim is to eliminate
          % edge effects in the filtering
        extension = (1:ceil(3*sigma))';
        
        dL1 = L(2)-L(1);
        dL2 = L(end)-L(end-1);
        Le = [-flipud(dL1*extension)+L(1); L;  dL2*extension+L(end)];
        
        Ls = filter2(G, Le);  % Apply smoothing filter
        
        % Trim off extensions
        Ls = Ls(length(extension)+1 : length(extension)+length(L));
    end
    
%----------------------------------------------------------------------------
% Delta E using the CIE76 formula + weighting

function deltaE = cie76(L, a, b, W)
    
    N = length(L);
    
    % Compute central differences 
    dL = zeros(size(L));
    da = zeros(size(a));
    db = zeros(size(b));

    dL(2:end-1) = (L(3:end) - L(1:end-2))/2;
    da(2:end-1) = (a(3:end) - a(1:end-2))/2;
    db(2:end-1) = (b(3:end) - b(1:end-2))/2;
    
    % Differences at end points
    dL(1) = L(2) - L(1);  dL(end) = L(end) - L(end-1);
    da(1) = a(2) - a(1);  da(end) = a(end) - a(end-1);
    db(1) = b(2) - b(1);  db(end) = b(end) - b(end-1);
    
    deltaE = sqrt(W(1)*dL.^2 + W(2)*da.^2 + W(3)*db.^2);
    
%----------------------------------------------------------------------------
% Delta E using the CIEDE2000 formula + weighting
%
% This function requires Gaurav Sharma's MATLAB implementation of the CIEDE2000
% color difference formula deltaE2000.m available at:
% http://www.ece.rochester.edu/~gsharma/ciede2000/

function deltaE = ciede2000(L, a, b, W)

    N = length(L);
    Lab = [L(:) a(:) b(:)];
    KLCH = 1./W;
    
    % Compute deltaE using central differences
    deltaE = zeros(N, 1);
    deltaE(2:end-1) = deltaE2000(Lab(1:end-2,:), Lab(3:end,:), KLCH)/2; 
   
    % Differences at end points    
    deltaE(1) = deltaE2000(Lab(1,:), Lab(2,:), KLCH);
    deltaE(end) = deltaE2000(Lab(end-1,:), Lab(end,:), KLCH);
    
